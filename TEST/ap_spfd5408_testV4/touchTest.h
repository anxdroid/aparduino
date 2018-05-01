#include <Wire.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>
#include <GraphicController.h>
#include <APIController.h>
#include <MainController.h>
#include <SolarController.h>

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
#define CMD_GET_LAST_RECV_SIZE    0x20

/*****************************************/
// JSON
/*****************************************/
JsonVariant jsonMasterDataPayload;
int jsonMasterCtrId = 0;
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
#define LOCK_TIMEOUT  10000

bool lock = false;
long lockLastTime = 0;
bool gettingusercmd = false;
bool toparseusercmd = false;
bool toreceive = false;
bool receiving = false;
bool parsing = false;
bool todecode = false;
bool decoding = false;
bool toconsume = false;
bool consuming = false;
bool tosend = false;
bool sending = false;
bool toclean = false;

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

/*****************************************/
// Controllers API
/*****************************************/
#define API_COUNT 256

APIController *ctrToRequest = NULL;
APIController *currentCtr = NULL;
String currentAction = "";
long lastCtrRequestTime = 0;







