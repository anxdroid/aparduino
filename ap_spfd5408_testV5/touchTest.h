#include <TimeLib.h>
#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>
#include <GraphicController.h>
#include <APIController2.h>
#include <MainController2.h>
#include <SolarController2.h>
#include <TempController2.h>
#include <SettingsController2.h>
#include <BackgroundController2.h>
#include <EmonCMSModel.h>

using namespace APLib;

/*****************************************/
// State machine statuses
/*****************************************/
#define LOCK_TIMEOUT  10000

bool lock = false;
long lockLastTime = 0;
bool gettingusercmd = false;

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

short TS_MINX = 150;
short TS_MINY = 139;
short TS_MAXX = 1020;
short TS_MAXY = 1007;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define PENRADIUS 3

TSPoint currTouchedPoint;

/*****************************************/
// Controllers API
/*****************************************/
#define API_COUNT 256

APIController2 *ctrList[API_COUNT];

int lastAPI = 0;

APIController2 *ctrToRequest = NULL;
APIController2 *currentCtr = NULL;
String currentAction = "";
long lastCtrRequestTime = 0;
char newCtrNameToRequestBuf[CTRNAMESIZE];
char ctrNameBuf[CTRNAMESIZE];

APIController2* getCtrByName(char* newCtrNameToRequestBuf) {
  APIController2* ctrToReq = currentCtr;
  for (int ctrId = 0; ctrId < API_COUNT; ctrId++) {
    APIController2* ctrPtr = ctrList[ctrId];
    if (ctrPtr != NULL) {

      ctrPtr->getCtrName(ctrNameBuf);
      //Serial.print("*");
      //Serial.println(ctrNameBuf);
      if (strcmp(ctrNameBuf, newCtrNameToRequestBuf) == 0) {
        //Serial.println("Found !");
        ctrToReq = ctrPtr;
        break;
      }
    }
  }
  //Serial.println("-------------------------------------");
  return ctrToReq;
}

void registerCtr(APIController2* ctrPtr) {
  ctrList[lastAPI] = ctrPtr;
  /*
    Serial.print("Registering");
    Serial.print(ctrPtr->getCtrName());
    Serial.print(" at position ");
    Serial.println(lastAPI);
  */
  lastAPI++;
}
GraphicController graphicCtr(Serial);

MainController2 mainCtr(&tft, &graphicCtr);
SolarController2 solarCtr(&tft, &graphicCtr);
TempController2 tempCtr(&tft, &graphicCtr);
SettingsController2 settingsCtr(&tft, &graphicCtr);
BackgroundController2 backgroundCtr(&tft, &graphicCtr);

EmonCMSModel emonCMSModel;











