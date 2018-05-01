#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 1, 12);
IPAddress serverCmd(192, 168, 1, 12);// numeric IP for Google (no DNS)
IPAddress ip(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress mydns(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const char* nodeid = "30";
const char* apikey = "2a4e7605bb826a82ef3a54f4ff0267ed";

EthernetClient client;

String id = "0";
String cmd = "";
String param = "";

String prev_id = "0";
String prev_cmd = "";
String prev_param = "";
OneWire  ds(6);
DallasTemperature sensors(&ds);

OneWire  ds2(2);
DallasTemperature sensors2(&ds2);

OneWire  ds3(3);
DallasTemperature sensors3(&ds3);
const int photoRes = A0;

const int analogIn = A4;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
int RawValue = 0;
int ACSoffset = 2500;
double Voltage = 0;
double Amps = 0;
/*
  { 0x28, 0xEE, 0x75, 0xA6, 0x1D, 0x16, 0x02, 0x9B } DISIMPEGNO
  { 0x28, 0xFF, 0x45, 0xB2, 0xA0, 0x16, 0x05, 0x6A } SOTTOTETTO
  { 0x28, 0xFF, 0xDD, 0xAA, 0xA0, 0x16, 0x05, 0xC7 } SALOTTO
*/

void printAddress(DeviceAddress deviceAddress)
{
  Serial.print("{ ");
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    Serial.print("0x");
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");

  }
  Serial.print(" }");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting.....");

  sensors.begin();
  sensors2.begin();
  sensors3.begin();

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  pinMode(5, OUTPUT);
  pinMode(photoRes, INPUT);

  pinMode(analogIn, INPUT);

  //if (Ethernet.begin(mac) == 0) {
  //  Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  //} else {
    Serial.println(Ethernet.localIP());
  //}
  //Serial.println("Accendo termosifoni");
  //digitalWrite(5, HIGH);
  delay(5000);
}


int parseCmd() {
  Serial.println(cmd);
  Serial.println(param);
  if (cmd == "HEATERS") {
    if (param == "ON") {
      Serial.println("Accendo termosifoni");
      digitalWrite(5, HIGH);
    } else {
      Serial.println("Spengo termosifoni");
      digitalWrite(5, LOW);
    }
  }
  return 0;
}

int parseClient() {
  int countLF = 0;
  bool payload = false;
  int n_token = 0;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\r') {
        countLF++;
      } else {
        if (c == '\n') {
          countLF += 10;
        } else {
          countLF = 0;
        }
      }
      if (countLF > 20) {
        payload = true;
        continue;
      }
      if (payload) {
        //Serial.print(countLF);
        //Serial.print(c);

        if (c == ' ') {
          n_token++;
        } else if (c == ':') {
          n_token++;
        } else if (c == '\r' || c == '\n') {
          n_token++;
        } else {
          if (n_token == 0) {
            id += c;
          } else if (n_token == 1) {
            cmd += c;
          } else if (n_token == 2) {
            param += c;
          }
        }


      }
      //if (c == " ") {
      //  Serial.println();
      //}
    }
  }
  if (payload) {
    return 0;
  } else {
    return 1;
  }
}

void httpGet(char* key, float value, char* unit) {
  if (client.connect(server, 80)) {
    Serial.print("Sending value...");
    Serial.print(key);
    Serial.print(" ");
    Serial.println(value);
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
    client.stop();
  } else {
    // kf you didn't get a connection to the server:
    Serial.print("connection failed to ");
    Serial.println(server);
  }
}

void loop()
{
  // ***************** Invio di dati acquisiti


  float currTime = millis();
  float lastTime = millis();
  double Current = 0;
  do {
    RawValue = abs(analogRead(analogIn));
    Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
    Amps = ((Voltage - ACSoffset) / mVperAmp);
    if (Amps > Current) {
      Current = Amps;
    }
    currTime = millis();
  } while ((currTime - lastTime) <= 20);
  Current /= sqrt(2);
  Serial.print("CURRENT_CASA: ");
  Serial.println(Current);
  httpGet("CURRENT_CASA", Current, "A");


  DeviceAddress tmp_address;
  // GPIO 6
  int numberOfDevices = sensors.getDeviceCount();
  sensors.requestTemperatures();
  for (int i = 0; i < 3; i++) {
    if (i > 0) {
      //Serial.print(" ");
    }
    float temp = sensors.getTempCByIndex(i);
    
    if (temp >= -5 && temp <= 45) {
      Serial.print("TEMP_SOTTOTETTO: ");
      Serial.println(temp);
      httpGet("TEMP_SOTTOTETTO", temp, "&deg;");
      sensors.getAddress(tmp_address, i);
      //printAddress(tmp_address);
      //Serial.println(temp);
      //Serial.print("sending value ");
      delay(1000);
    }else{
      //Serial.print("discarding value ");
    }
    
  }
  // GPIO 5
  numberOfDevices = sensors2.getDeviceCount();
  sensors2.requestTemperatures();
  for (int i = 0; i < 3; i++) {
    if (i > 0) {
      //Serial.print(" ");
    }
    float temp = sensors2.getTempCByIndex(i);
    
    if (temp >= -5 && temp <= 45) {
      Serial.print("TEMP_DISIMPEGNO: ");
      Serial.println(temp);
      httpGet("TEMP_DISIMPEGNO", temp, "&deg;");
      sensors2.getAddress(tmp_address, i);
      //printAddress(tmp_address);
      //Serial.print("sending value ");
      delay(1000);
    }else{
      //Serial.print("discarding value ");
    }
    
  }
  // GPIO 3
  numberOfDevices = sensors3.getDeviceCount();
  sensors3.requestTemperatures();
  for (int i = 0; i < 3; i++) {
    if (i > 0) {
      //Serial.print(" ");
    }
    float temp = sensors3.getTempCByIndex(i);
    
    if (temp >= -5 && temp <= 45) {
      Serial.print("TEMP_SALOTTO: ");
      Serial.println(temp);
      httpGet("TEMP_SALOTTO", temp, "&deg;");
      sensors3.getAddress(tmp_address, i);
      //printAddress(tmp_address);
      //Serial.print("sending value ");
      delay(1000);
    }else{
      //Serial.print("discarding value ");
    }
    
  }
  Serial.println();
/*
  int lightval = 1023 - analogRead(photoRes);
  httpGet("LIGHT_SOTTOTETTO", lightval, "&perc;");
  Serial.println(lightval);
*/

  // ***************** Richiesta di comandi

  if (client.connect(serverCmd, 80)) {
    client.println("GET /temp/jobs.php?req_cmd=HEATERS&simple_out=1 HTTP/1.1");
    client.println("Host: 192.168.1.12:80");
    client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
    client.println("Connection: close");
    client.println();
    int retval = parseClient();
    client.stop();
    if (retval == 0) {
      /*
        Serial.println("--------");
        Serial.println(id);
        Serial.println(cmd);
        Serial.println(param);
      */
      int retcmd = parseCmd();
      if (retcmd == 0) {
        //Serial.print("connecting...");
        if (client.connect(serverCmd, 80)) {
          //Serial.println("connected");
          client.print("GET /temp/jobs.php?job_id=");
          client.print(id);
          client.println(" HTTP/1.1");
          client.println("Host: 192.168.1.12:80");
          client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
          client.println("Connection: close");
          client.println();
          client.stop();
        }
      }
      prev_id = id;
      prev_cmd = cmd;
      prev_param = param;
      id = cmd = param = "";
    }
  }
  delay(2000);
}
