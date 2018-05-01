#include "touchTest.h"

APIController *ctrList[API_COUNT];
TSPoint currTouchedPoint;

bool decodeJsonMasterData() {
  decoding = true;
  toconsume = false;
  StaticJsonBuffer<4096> jsonBuffer;
  // Parse the root object
  JsonObject &jsonResponse = jsonBuffer.parseObject(masterData);
  
  if (!jsonResponse.success()) {
    //Serial.println(masterData);
    Serial.println(F("Failed to read file, using default configuration"));
  }else{
    masterData = true; 
    jsonMasterCtrId = jsonResponse["i"];
    jsonMasterTSStart = jsonResponse["s"];
    //Serial.print("Needed ");
    //Serial.print(currentCtr->getCtrId());
    //Serial.print(" got ");
    //Serial.println(jsonMasterCtrId);
    jsonMasterDataPayload = jsonResponse["p"];
    toconsume = true;
  }
  decoding = false;
  return toconsume;
}

bool consumeMasterData() {
    consuming = true;
    currentCtr->actionCalled(&jsonMasterDataPayload, jsonMasterTSStart, currentAction);
    currentAction = "";
    consuming = false;
    toclean = true;
}

void setup(void) {
  Serial.begin(9600);
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveRequest);
  Wire.onRequest(sendResponse);

  GraphicController graphicCtr(Serial);
  
  for (int i = 0; i < API_COUNT; i++) {
    ctrList[i] = NULL;
  }

  NOOPController noopCtr(&tft, ctrList, &graphicCtr, Serial);
  NOOPController* noopCtrPtr = &noopCtr;
  ctrList[noopCtrPtr->getCtrId()] = noopCtrPtr;
  
  MainController mainCtr(&tft, ctrList, &graphicCtr, Serial);
  MainController* mainCtrPtr = &mainCtr;
  ctrList[mainCtrPtr->getCtrId()] = mainCtrPtr;

  SolarController solarCtr(&tft, ctrList, &graphicCtr, Serial);
  SolarController* solarCtrPtr = &solarCtr;
  ctrList[solarCtrPtr->getCtrId()] = solarCtrPtr;
  
  // Main Controller
  currentCtr = ctrList[1];
  // NOOP Controller
  ctrToRequest = ctrList[0];
    
  width = tft.width() - 1;
  height = tft.height() - 1;
  currentCtr->index();
}

void loop(void) {
  getUserCmd();
  if (todecode) {
    toconsume = decodeJsonMasterData();
    todecode = false;
  }
  if (toconsume) {
    consumeMasterData();
    // NOOP Controller
    ctrToRequest = ctrList[0];
    toconsume = false;
    unsetLock("done parsing");
  }
  if (toclean) {
    // NOOP Controller
    ctrToRequest = ctrList[0];
    toclean = false;
  }
}

bool doLock() {
  if (lock) {
    long currTime = millis();
    if (currTime - lockLastTime >= LOCK_TIMEOUT) {
      unsetLock("timeout");
    }
  }  
  return lock;
}

void unsetLock(String msg) {
  lock = false;
  ctrToRequest = ctrList[0];
  if (msg != "") {
    //Serial.print("unlock: ");
    //Serial.println(msg);
  }
}

void setLock() {
  lock = true;
  lockLastTime = millis();
  //Serial.println("lock !"); 
}

void getUserCmd() {
  if (doLock()) {
    return;
  }

  gettingusercmd = true;
  
  digitalWrite(13, HIGH);
  currTouchedPoint.z = 0;
  currTouchedPoint = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(YP, OUTPUT);     //.kbv these pins are shared with TFT
  pinMode(XM, OUTPUT);     //.kbv these pins are shared with TFT
  //Serial.println(ctrToRequest->getCtrId());
  
  if (currTouchedPoint.z > 0) {
    //Serial.println(currentCtr->getCtrName());
    currTouchedPoint.x = map(currTouchedPoint.x, TS_MINX, TS_MAXX, 0, tft.width());
    currTouchedPoint.y = map(currTouchedPoint.y, TS_MINY, TS_MAXY, 0, tft.height());
    //Serial.print(currTouchedPoint.x);
    //Serial.print(",");
    //Serial.println(currTouchedPoint.y);
    int newCtrIdToRequest = currentCtr->mapUserCmd(&currTouchedPoint);
    //Serial.println(newCtrIdToRequest);
    APIController *newCtrToRequest = ctrList[newCtrIdToRequest];   
    // Safety...
    if (newCtrToRequest == NULL) {
      newCtrToRequest = ctrList[1];
    }
    // Too fast ?
    long interTime = millis() - lastCtrRequestTime;    
    if (ctrToRequest->getCtrId() != newCtrIdToRequest || interTime > 150) {
      ctrToRequest = newCtrToRequest;
      //Serial.print("Requested: ");
      //Serial.println(ctrToRequest->getCtrId());
      setLock();
      lastCtrRequestTime = millis();
    }   
  }

  // Init controller
  if (ctrToRequest->getCtrId() > 0) {
    currentCtr = ctrToRequest;
    Serial.print("Switched to ");
    Serial.println(currentCtr->getCtrName());
    currentCtr->index();
    // No default request
    ctrToRequest = ctrList[0];
  }  
  gettingusercmd = false;
}

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
  //Serial.print("Command: ");
  //Serial.println(masterCmd);
  
  // Data
  if (masterCmd == CMD_LAST_CHUNK || masterCmd == CMD_INTERMEDIATE_CHUNK || masterCmd == CMD_GET_LAST_RECV_SIZE) {
    if (masterCmd == CMD_LAST_CHUNK) {
      if (masterData != "") {
        todecode = true;
        masterDataSize = masterData.length();
        Serial.print("Got ");
        Serial.print(masterDataSize);
        Serial.println(" bytes");
      }else{
        setLock();
      }
    }
    if (masterCmd == CMD_GET_LAST_RECV_SIZE) {
        //Serial.println("waiting response...");
        slaveResponse = (String)masterDataSize;
        masterDataSize = 0;
        tosend = true;
    }
  // Commands
  }else{
    masterData = "";   
    if (masterCmd == CMD_GET_TOUCH_CMD) {
      // Actions called from within the controllers
      if (currentCtr != NULL && strcmp(currentCtr->getActionToRequest(), "") != 0) {
        //Serial.print("Action: ");
        //Serial.println(currentCtr->getActionToRequest());
        slaveResponse = currentCtr->getActionToRequest();
        currentAction = slaveResponse;
      }else{
        slaveResponse = "0";
        unsetLock("");
      }
      tosend = true;
    }else{
      Serial.print("Unrecognized command: ");
      Serial.println(masterCmd);
      slaveResponse = "0";
      tosend = true;
      
    }
  }
  parsing = false;
}

void sendResponse() {
  //Serial.println("---------------------------");
  //Serial.println("sendResponse");
  //Serial.println("---------------------------");   
  while (!tosend);
  sending = true;
  if (slaveResponse != "0") { 
    //Serial.print("Sending: ");
    //Serial.println(slaveResponse);
  }
  Wire.write(slaveResponse.c_str());
  sending = false;
  toclean = true;
  tosend = false;
}








