/*
  Sketch which publishes temperature data from a DS1820 sensor to a MQTT topic.
  This sketch goes in deep sleep mode once the temperature has been sent to the MQTT
  topic and wakes up periodically (configure SLEEP_DELAY_IN_SECONDS accordingly).
  Hookup guide:
  - connect D0 pin to RST pin in order to enable the ESP8266 to wake up periodically
  - DS18B20:
     + connect VCC (3.3V) to the appropriate DS18B20 pin (VDD)
     + connect GND to the appopriate DS18B20 pin (GND)
     + connect D4 to the DS18B20 data pin (DQ)
     + connect a 4.7K resistor between DQ and VCC.
*/

#include <ESP8266WiFi.h>

#define SLEEP_DELAY_IN_SECONDS  10
#define ONE_WIRE_BUS            D5      // DS18B20 pin

int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
int RawValue = 0;
int ACSoffset = 2500;
double Voltage = 0;
double Amps = 0;

const char* ssid = "apmain";
const char* password = "montefalconedivalfortore2014";
WiFiClient client;
const char* server = "192.168.1.3";
const char* nodeid = "40";
const char* apikey = "2a4e7605bb826a82ef3a54f4ff0267ed";
//IPAddress server(192, 168, 1, 3);

void setup() {
  // setup serial port
  Serial.begin(115200);

  // setup WiFi
  setup_wifi();

  pinMode(A0, INPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

float getCurrent() {
  float currTime = millis();
  float lastTime = millis();
  double Current = 0;
  do {
    RawValue = abs(analogRead(A0));
    Voltage = (RawValue / 1024.0) * 3000; // Gets you mV
    Amps = ((Voltage - ACSoffset) / mVperAmp);
    if (Amps > Current) {
      Current = Amps;
    }
    currTime = millis();
  } while ((currTime - lastTime) <= 20);
  Current /= sqrt(2);
  return Current;
}

void httpGet(char* key, float value, char* unit) {
  if (client.connect(server, 80)) {
    Serial.println("Sending value...");
    client.print("GET /emoncms/input/post.json?apikey=");
    client.print(apikey);
    client.print("&node=");
    client.print(nodeid);
    client.print("&json={");
    client.print(key);
    client.print(":");
    client.print(value);
    client.print("}");
    client.println(" HTTP/1.1");
    client.println("Accept: */*");
    client.print("Host:");
    client.print(server);
    client.println(":80");
    //client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
    client.println("Connection: close");
    client.println();
  }
}

void loop() {
  float Current = getCurrent();
  Serial.print("Got current: ");
  Serial.println(Current);
  httpGet("CURRENT_TERMO", Current, "A");

  //ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  delay(10000);   // wait for deep sleep to happen
}
