// *** SPFD5408 change -- Begin
#include <Wire.h>
#include <ArduinoJson.h>
#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>
// *** SPFD5408 change -- End

String recv;
String data;

String cmdToSend[5];
char* cmdRecvd[5];
#define HM_CMD            0
#define HM_ID_CMD_MASTER  1
#define HM_ID_CMD_SLAVE   2
#define HM_STATUS         3
#define HM_RESPONSE       4

bool receiving = false;
bool toprint = false;
bool printing = false;
bool tosend = false;
bool sending = false;
bool todecode = false;
bool decoding = false;
bool toparse = false;
bool parsing = false;

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define SLAVE_ADDRESS 0x04

// Era A1
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
// Era 7
#define YM 9   // can be a digital pin
// Era 8
#define XP 8   // can be a digital pin

short TS_MINX=180;
short TS_MINY=138;
short TS_MAXX=1034;
short TS_MAXY=1008;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void) {
  Serial.begin(9600);
  Serial2.begin(9600);  
  //progmemPrintln(PSTR("TFT LCD test"));
  Wire.begin(SLAVE_ADDRESS);
  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  //Wire.onRequest(sendData);

  cmdToSend[HM_CMD] = "";
  cmdToSend[HM_ID_CMD_MASTER] = "";
  cmdToSend[HM_ID_CMD_SLAVE] = "";
  cmdToSend[HM_STATUS] = "";
  cmdToSend[HM_RESPONSE] = "";

  
#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  //progmemPrintln(PSTR("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
#else
  //progmemPrintln(PSTR("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
#endif
  tft.reset();
  tft.begin(0x9341); // SDFP5408
  tft.setRotation(0); // Need for the Mega, please changed for your choice or rotation initial
  // *** SPFD5408 change -- End
  //progmemPrintln(PSTR("Benchmark                Time (microseconds)"));
  //progmemPrint(PSTR("Screen fill              "));
  //Serial.println(testFillScreen());
  //delay(500);

  //progmemPrint(PSTR("Text                     "));
  //Serial.println(testText());
  //delay(3000);
/*
  if (Serial2.available()) {
    Serial.println("garbage received !");
    String recv = Serial2.readString();
    Serial.println(recv);
  }
  if (Serial2.available()) {
    Serial.println("garbage received !");
    String recv = Serial2.readString();
    Serial.println(recv);
  }
*/  
}

void loop(void) {
  testSerialInput();
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}

unsigned long testSerialInput() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(5);
  tft.setTextColor(WHITE);
/*
  if (Serial2.available()) {
    Serial.println("msg received !");
    recv = Serial2.readString();
    Serial.println(recv);
  }
  printRecv();
*/  
  return micros() - start;  
}

unsigned long testText() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial1.println();
}

// callback for received data
void receiveData(int byteCount) {
  int avail = Wire.available();
  if (Wire.available()) {
    //Serial.println("---------------------------");
    //Serial.println("receiveData");
    //Serial.println("---------------------------");
    byte cmd = 0xff;
    data = "";
    int nchar = 0;
    while (Wire.available()) {
      receiving = true;
      if (nchar == 0) {
        //Serial.println(avail+" bytes available");
        cmd = Wire.read();
        //Serial.println("Reading flag: "+cmd);
      }else{
        char c = (char)Wire.read();
        //Serial.println(c);
        data += c;
      }
      nchar++;  
    }

    recv += data;
    
    //Serial.println(data.length()+" bytes readable: "+data);
    Serial.print(" - Got: ");
    Serial.println(data);
    // Last chunk
    if (cmd == 0x00) {
      if (recv != "") {
        toprint = true;
      }
      Serial.print("Done: ");
      Serial.println(recv);
      bool res = parseJson();
      
    }else{
      Serial.println("Waiting...");
    }
    receiving = false;
    tosend = true;
    //sendData();
  }
}

bool parseJson() {
  StaticJsonBuffer<512> jsonBuffer;
  // Parse the root object
  JsonObject &root = jsonBuffer.parseObject(recv);
  if (!root.success())
    Serial.println(F("Failed to read file, using default configuration"));
  else{
/*
    const char* sensor = root["sensor"];
    long time          = root["time"];
    double latitude    = root["data"][0];
    double longitude   = root["data"][1];
    Serial.println(sensor);
    Serial.println(time);
    Serial.println(latitude);
    Serial.println(longitude);
*/    
    
    cmdRecvd[HM_CMD] = root["cmd"];
    cmdRecvd[HM_ID_CMD_MASTER] = root["idCmdMaster"];
    cmdRecvd[HM_ID_CMD_SLAVE] = root["idCmdSlave"];
    cmdRecvd[HM_STATUS] = root["status"];
    cmdRecvd[HM_RESPONSE] = root["response"];
    return true;
  }
  return false;
}

void printRecv() { 
  if (toprint && recv != "") {
    Serial.println("---------------------------");
    Serial.println("printRecv");
    Serial.println("---------------------------");     
    printing = true;
    tft.fillScreen(BLACK);
    tft.setCursor(10, 10);
    tft.setTextSize(5);
    tft.setTextColor(RED);
    tft.println(recv);
    //myDelay (2000);
    delay(2000);
    Serial.println(recv);
    recv = "";
    printing = false;
  }
  toprint = false;
}

// callback for sending data
void sendData() {
  if (tosend) {
    Serial.println("---------------------------");
    Serial.println("sendData");
    Serial.println("---------------------------");     
    sending = true;
    Serial.print(data.length());
    Serial.println(" bytes received");
    Wire.write(data.length());
    sending = false;
  }
  tosend = false;
}


