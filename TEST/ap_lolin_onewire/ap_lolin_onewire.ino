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
#include <OneWire.h>
#include <DallasTemperature.h>

#define SLEEP_DELAY_IN_SECONDS  30
#define ONE_WIRE_BUS            D5      // DS18B20 pin

const char* ssid = "apmain";
const char* password = "montefalconedivalfortore2014";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

char temperatureString[6];

void setup() {
  // setup serial port
  Serial.begin(115200);

  // setup WiFi
  setup_wifi();

  // setup OneWire bus
  DS18B20.begin();
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

float getTemperature(int i) {
  Serial.println("Requesting DS18B20 temperature...");
  float temp;
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(i);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void loop() {
  int devices = DS18B20.getDeviceCount();
  Serial.print("Found ");
  Serial.print(devices);
  Serial.println(" devices");
  for (int i = 0; i < devices; i++) {
    float temperature = getTemperature(i);
    // convert temperature to a string with two digits before the comma and 2 digits for precision
    dtostrf(temperature, 2, 2, temperatureString);
    // send temperature to the serial console
    Serial.print("Got temperature: ");
    Serial.println(temperatureString);
  }
  //ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  delay(2000);   // wait for deep sleep to happen
}
