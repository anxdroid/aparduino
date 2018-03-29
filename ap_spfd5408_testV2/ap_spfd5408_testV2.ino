#include <APIController.h>
#include <GraphicController.h>

#include <MainController.h>


#include <Wire.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>

using namespace APLib;

/*****************************************/
// I2C
/*****************************************/
#define SLAVE_ADDRESS 0x04

/*****************************************/
// Master commands
/*****************************************/
byte masterCmd = 0xff;
#define CMD_LAST_CHUNK            0x00
#define CMD_INTERMEDIATE_CHUNK    0x01
#define CMD_GET_TOUCH_CMD         0x10

/*****************************************/
// JSON
/*****************************************/
JsonVariant jsonMasterDataPayload;
int jsonMasterApiId = 0;
long jsonMasterTSStart = 0;

/*****************************************/
// Buffers
/*****************************************/
String data;
String masterData;
int masterDataSize = 0;
String slaveResponse;

/*****************************************/
// State machine statuses
/*****************************************/
bool lock = false;
bool gettingusercmd = false;
bool toparseusercmd = false;
bool toreceive = false;
bool receiving = false;
bool toparse = false;
bool parsing = true;
bool todecode = false;
bool decoding = false;
bool toconsume = false;
bool consuming = false;
bool tosend = false;
bool sending = false;
bool toprint = false;
bool printing = false;

/*****************************************/
// TFT
/*****************************************/
uint16_t width = 0;
uint16_t height = 0;

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

short TS_MINX=150;
short TS_MINY=139;
short TS_MAXX=1020;
short TS_MAXY=1007;

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

#define PENRADIUS 3

/*****************************************/
// API
/*****************************************/
typedef int (*pFT) ();
struct API {
  int id;
  //String api;
  bool getData;
  pFT parsingFunction;
};

APIController *lastRequestedCtr = NULL;
APIController *ctrToRequest = NULL;

long lastApiRequestTime = 0;

API apiToRequest;
API lastRequestedApi;

#define API_NULL                  API{0, false, NULL}
#define API_TEST1                 API{1, false, NULL}
#define API_TEST2                 API{2, false, NULL}

int parsing3() {
  Serial.println("parsing3 ready to parse !");
  int chartWidth = 320;
  int chartHeight = 240;
  int leftMargin = 10;
  int topMargin = 10;
  int bottomMargin = 40;
  int rightMargin = 20;
  int labelSize = 2;  
  drawChart(chartWidth, chartHeight, topMargin, leftMargin, bottomMargin, rightMargin, labelSize);
  return 0;
}

#define API_TEST3                 API{3, true, &parsing3}
#define API_TEST4                 API{4, false, NULL}
#define API_TEST10                API{10, true, NULL}

APIController *apiList[256];

void setup(void) {
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveRequest);
  Wire.onRequest(sendResponse);

  apiToRequest = lastRequestedApi = API_NULL;

  GraphicController graphicCtr(&tft, &jsonMasterDataPayload, jsonMasterTSStart);
  apiList[0] = new MainController(&tft, apiList, &graphicCtr);
  lastRequestedCtr = ctrToRequest = apiList[0];
    
  width = tft.width() - 1;
  height = tft.height() - 1;
  
  /*
  tft.reset();
  tft.begin(0x9341); // SDFP5408
  tft.setRotation(3); // Need for the Mega, please changed for your choice or rotation initial 

  tft.fillScreen(BLACK);
  tft.drawLine(tft.width()/2, 0, tft.width()/2, tft.height(), WHITE);
  tft.drawLine(0, tft.height()/2, tft.width(), tft.height()/2, WHITE);
  int labelSize = 2;
  String label = "Solar";
  tft.setCursor((3*tft.width()/4)-(label.length()*labelSize*3), (tft.height()/4)-(labelSize*5));
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.println(label);
  */
  ctrToRequest->drawPage();
}

void loop(void) {
  getUserCmd();

  if (toparse) {
    tft.fillScreen(BLACK);      
    tft.setCursor(tft.width()/2, tft.height()/2);
    tft.setTextSize(5);
    tft.setTextColor(WHITE);
    tft.println("Wait !");
    lock = true;
    parseRequest();
    toparse = false;
  }
  if (todecode) {
    decodeJsonMasterData();
    todecode = false;
    masterDataSize = masterData.length();
    masterData = "";
    slaveResponse = (String)masterDataSize;
  }
  if (toconsume) {
    consumeMasterData();
    toconsume = false;
    lock = false;
  }
  if (toprint) {
    printRequest();
    toprint = false;
  }
}

bool doing() {
  if (gettingusercmd || receiving || decoding || decoding || consuming) {
    return true;
  }
  return false;
}

/*****************************************/
// Graphic
/*****************************************/

void drawChart(int chartWidth, int chartHeight, int topMargin, int leftMargin, int bottomMargin, int rightMargin, int labelSize) {
  tft.fillScreen(BLACK);

  if (chartWidth+leftMargin+rightMargin > tft.width()) {
      chartWidth = tft.width() - leftMargin - rightMargin;
  }
  if (chartHeight+topMargin+bottomMargin > tft.height()) {
      chartHeight = tft.height() - topMargin - bottomMargin;
  }
  //Serial.print("Chart size: ");
  Serial.print(chartWidth);
  Serial.print("x");
  Serial.println(chartHeight);
  
  TSPoint chartTopLeft(leftMargin, topMargin, 0);
  TSPoint chartBottomRight(leftMargin+chartWidth, topMargin+chartHeight, 0);
  
  tft.drawRect(chartTopLeft.x, chartTopLeft.y, chartBottomRight.x-10, chartBottomRight.y-10, WHITE);
  long minTS = NULL;
  long maxTS = NULL;
  double minVal = NULL;
  double maxVal = NULL;
  
  for( JsonVariant& element : jsonMasterDataPayload.as<JsonArray>() ) {
    JsonArray* measurement = &element.as<JsonArray>();
    double value = (*measurement)[1];
    long offset = (*measurement)[0];
    time_t timestamp = offset + jsonMasterTSStart;
    if (minTS == NULL || timestamp < minTS ) {
      minTS = timestamp;
    }
    if (maxTS == NULL || timestamp > maxTS ) {
      maxTS = timestamp;
    }    
    if (minVal == NULL || value < minVal ) {
      minVal = value;
    }
    if (maxVal == NULL || value > maxVal ) {
      maxVal = value;
    }
  }

  tft.setTextSize(labelSize);
  tft.setTextColor(WHITE);
  String timeLabel = "";
  timeLabel += hour(minTS);
  timeLabel += ":";
  timeLabel += minute(minTS);
  tft.setCursor(chartTopLeft.x, chartBottomRight.y+10);
  tft.println(timeLabel);

  timeLabel = "";
  timeLabel += hour(maxTS);
  timeLabel += ":";
  timeLabel += minute(maxTS);
  int labelWidth = labelSize * 6 * timeLabel.length();
  tft.setCursor(chartBottomRight.x-labelWidth, chartBottomRight.y+10);  
  tft.println(timeLabel);

  tft.setCursor(chartTopLeft.x+5, chartBottomRight.y-20);
  tft.println(minVal);  

  tft.setCursor(chartTopLeft.x+5, chartTopLeft.y+5);
  tft.println(maxVal);

  TSPoint previousPoint(0, 0, 0);
  long previousDataColor = 0;
  int pointCount = 0;
  for( JsonVariant& element : jsonMasterDataPayload.as<JsonArray>() ) {
    JsonArray* measurement = &element.as<JsonArray>();
    char* color = (*measurement)[2];
    double value = (*measurement)[1];
    long offset = (*measurement)[0];
    
    time_t timestamp = offset + jsonMasterTSStart;
    
    long dataColor = 0;
    //Serial.println(color);
    if (*color == 'Y') {
      dataColor = YELLOW;
    }
    if (*color == 'B') {
      dataColor = BLUE;
    }
    if (*color == 'W') {
      dataColor = WHITE;
    }
    if (*color == 'R') {
      dataColor = RED;
    }
    //Serial.println(dataColor);
    
    double diff = timestamp-minTS;
    double ratio = diff/(maxTS-minTS);
    int x = chartTopLeft.x+((chartBottomRight.x-chartTopLeft.x)*ratio);
        
    diff = value-minVal;
    //Serial.println(diff);
    ratio = diff/(maxVal-minVal);
    //Serial.println(ratio);
    
    int y = chartBottomRight.y - ((chartBottomRight.y-chartTopLeft.y)*ratio);
    //tft.drawLine(x, y, x, chartBottomRight.y, dataColor);
    if (pointCount > 0 && dataColor == previousDataColor) {
      tft.drawLine(x, y, previousPoint.x, previousPoint.y, dataColor);
    }
    previousPoint.x = x;
    previousPoint.y = y;
    previousDataColor = dataColor;
    pointCount++;
  }  
}

API mapUserCmd(TSPoint p) {
  // Map user request to API
  API newApiToRequest = API_TEST1;
  //int color = RED;
  if (p.x < tft.width()/2) {
    //color = BLUE;
    newApiToRequest = API_TEST2;
    if (p.y < tft.width()/2) {
      //color = WHITE;
      newApiToRequest = API_TEST3;
    }
  }else{
    if (p.y < tft.width()/2) {
      //color = YELLOW;
      newApiToRequest = API_TEST10;
    }
  }
  return newApiToRequest;
}

void getUserCmd() {
  gettingusercmd = true;
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(YP, OUTPUT);     //.kbv these pins are shared with TFT
  pinMode(XM, OUTPUT);     //.kbv these pins are shared with TFT
  apiToRequest = API_NULL;
  if (p.z > 0) {
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    tft.fillCircle(p.x, p.y, 10, BLACK);
    API newApiToRequest = mapUserCmd(p);
    // Too fast ?
    long interTime = millis() - lastApiRequestTime;
    if (lastRequestedApi.id != newApiToRequest.id || interTime > 150) {
      apiToRequest = lastRequestedApi = newApiToRequest;
      lastApiRequestTime = millis();
    }else{
      apiToRequest = API_NULL;
    }
  }
  gettingusercmd = false;
}

// callback for received data
void receiveRequest(int byteCount) {
  if (Wire.available()) {
    //Serial.println("---------------------------");
    //Serial.println("receiveRequest");
    //Serial.println("---------------------------");
    masterCmd= 0xff;
    data = "";
    int nchar = 0;
    while (Wire.available()) {
      receiving = true;
      if (nchar == 0) {
        masterCmd= Wire.read();
      }else{
        char c = (char)Wire.read();
        data += c;
      }
      nchar++;  
    }
    //Serial.println(data);
    masterData += data;
    parseRequest();
    receiving = false;
  }
}

void parseRequest() {
    parsing = true;
    if (masterCmd == CMD_LAST_CHUNK || masterCmd == CMD_INTERMEDIATE_CHUNK) {
      if (masterCmd == CMD_LAST_CHUNK) {
        if (masterData != "") {
          todecode = true;
        }
      }else if (masterCmd == CMD_INTERMEDIATE_CHUNK) {
        //Serial.println("Waiting for more chunks...");
      }
    }else{
      masterData = "";
      if (masterCmd == CMD_GET_TOUCH_CMD) {
        //Serial.println("Touchscreen poll");
        slaveResponse = apiToRequest.id;
        //slaveResponse = 
        tosend = true;
      }
    }
    parsing = false;
}

bool decodeJsonMasterData() {
  decoding = true;
  toconsume = false;
  StaticJsonBuffer<4096> jsonBuffer;
  // Parse the root object
  JsonObject &jsonResponse = jsonBuffer.parseObject(masterData);
  
  if (!jsonResponse.success()) {
    Serial.println(masterData);
    Serial.println(F("Failed to read file, using default configuration"));
  }else{    
    jsonMasterApiId = jsonResponse["i"];
    jsonMasterTSStart = jsonResponse["s"];
    Serial.print("Needed ");
    Serial.print(lastRequestedApi.id);
    Serial.print(" got ");
    Serial.println(jsonMasterApiId);
    jsonMasterDataPayload = jsonResponse["p"];
    toconsume = true;
  }
  decoding = false;
  return toconsume;
}

bool consumeMasterData() {
    //Serial.println("---------------------------");
    //Serial.println("consumeMasterData");
    //Serial.println("---------------------------");
    consuming = true;
    
    if (lastRequestedApi.getData && lastRequestedApi.id == jsonMasterApiId) {
      (*(lastRequestedApi.parsingFunction))();
    }
    
    tosend = true;
    consuming = false;
}

void printRequest() { 
  //Serial.println("---------------------------");
  Serial.println("printRequest");
  Serial.println("---------------------------");     
  printing = true;
  tft.fillScreen(BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(5);
  tft.setTextColor(RED);
  tft.println(masterData);
  //myDelay (2000);
  delay(2000);
  Serial.println(masterData);
  printing = false;
}

// callback for sending data
void sendResponse() {
  //Serial.println("---------------------------");
  //Serial.println("sendResponse");
  //Serial.println("---------------------------");   
  if (tosend || doing()) {
    while (!tosend);
    sending = true;
    //String slaveResponse = "{"+(String)HM_API_ID+":"+jsonMasterApiId+", "+HM_STATUS+":200, "+HM_RESPONSE+":"+slaveResponse+"}";
    Wire.write(slaveResponse.c_str());
    sending = false;
  }
  tosend = false;
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


