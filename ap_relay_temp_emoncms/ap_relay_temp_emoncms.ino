#include <OneWire.h>
#include <DallasTemperature.h>

const char* nodeid = "30";
const char* apikey = "2a4e7605bb826a82ef3a54f4ff0267ed";

String id = "0";
String cmd = "";
String param = "";

String prev_id = "0";
String prev_cmd = "";
String prev_param = "";
OneWire  ds(6);
DallasTemperature sensor1(&ds);

OneWire  ds2(2);
DallasTemperature sensor2(&ds2);

OneWire  ds3(3);
DallasTemperature sensor3(&ds3);
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

  sensor1.begin();
  sensor2.begin();
  sensor3.begin();

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  
  pinMode(photoRes, INPUT);

  pinMode(analogIn, INPUT);

  Serial.println("Accendo termosifoni");
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  delay(5000);
}

String parseCmd() {
  //Serial.println(cmd);
  //Serial.println(param);
  String returnVal;
  if (cmd == "HEATERS") {
    if (param == "ON") {
      //Serial.println("Accendo termosifoni");
      returnVal = "ON";
      digitalWrite(5, HIGH);
    } else {
      //Serial.println("Spengo termosifoni");
      returnVal = "OFF";
      digitalWrite(5, LOW);
    }
  }
  return returnVal;
}

void loop()
{
  // ***************** Consumo corrente

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
  Serial.print(nodeid);
  Serial.print(":CURRENT_CASA:");
  Serial.print(Current);
  Serial.println(":A");

  // ***************** Temperature  
  //DeviceAddress tmp_address;

  // GPIO 6: TEMP_SOTTOTETTO
  int numberOfDevices = sensor1.getDeviceCount();
  sensor1.requestTemperatures();
  for (int i = 0; i < 3; i++) {
    float temp = sensor1.getTempCByIndex(i);
    if (temp >= -5 && temp <= 45) {
      Serial.print(nodeid);
      Serial.print(":TEMP_SOTTOTETTO:");
      Serial.print(temp);
      Serial.println(":&deg;");
      //sensors.getAddress(tmp_address, i);
      //printAddress(tmp_address);
      //delay(1000);
    }
  }
  // GPIO 5: TEMP_DISIMPEGNO
  numberOfDevices = sensor2.getDeviceCount();
  sensor2.requestTemperatures();
  for (int i = 0; i < 3; i++) {
    float temp = sensor2.getTempCByIndex(i);
    if (temp >= -5 && temp <= 45) {
      Serial.print(nodeid);
      Serial.print(":TEMP_DISIMPEGNO:");
      Serial.print(temp);
      Serial.println(":&deg;");
      //sensor2.getAddress(tmp_address, i);
      //printAddress(tmp_address);
      //delay(1000);
    }
  }
  // GPIO 3: TEMP_SALOTTO
  numberOfDevices = sensor3.getDeviceCount();
  sensor3.requestTemperatures();
  for (int i = 0; i < 3; i++) {
    float temp = sensor3.getTempCByIndex(i);
    
    if (temp >= -5 && temp <= 45) {
      Serial.print(nodeid);
      Serial.print(":TEMP_SALOTTO:");
      Serial.print(temp);
      Serial.println(":&deg;");
      //sensor3.getAddress(tmp_address, i);
      //printAddress(tmp_address);
      //delay(1000);
    }
  }

  // ***************** Illuminazione
/*
  int lightval = 1023 - analogRead(photoRes);
  httpGet("LIGHT_SOTTOTETTO", lightval, "&perc;");
  Serial.println(lightval);
*/

  // ***************** Richiesta di comandi
  //Serial.flush();
  Serial.println("0:GET_CMD:0:X");
  Serial.flush();
  //while (Serial.available() == 0);
  id = "";
  cmd = "";
  param = "";
  String tmpToken = "";
  String recvData = "";
  int nToken = 0;
  while (Serial.available() == 0);

  if (Serial.available() > 0) {
    recvData = Serial.readString();
  }

  if (recvData != "") {
    for (int i = 0; i < recvData.length(); i++) {
      
      if (recvData.charAt(i) != ' ' && nToken == 0) {
        id += recvData.charAt(i);
      }else if (recvData.charAt(i) == ' ' && nToken == 0) {
        nToken++;
      }else if (recvData.charAt(i) != ':' && nToken == 1) {
        cmd += recvData.charAt(i);
      }else if (recvData.charAt(i) == ':' && nToken == 1) {
        nToken++;
      }else if (recvData.charAt(i) != '\r' && recvData.charAt(i) != '\n' nToken == 2) {
        param += recvData.charAt(i);
      }
    }    
    /*
    Serial.println(recvData);
    Serial.print(id);
    Serial.print(") cmd: ");
    Serial.print(cmd);
    Serial.print(" param: ");
    Serial.println(param);
    */
    if (cmd != "") {
      String returnVal = parseCmd();
      Serial.print(id);
      Serial.print(":");
      Serial.println(returnVal);
      Serial.flush();
    }
  }
  //delay(2000);
}

