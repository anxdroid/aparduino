#include "touchTest.h"

APIController *apiList[API_COUNT];
TSPoint currTouchedPoint;

void setup(void) {
  Serial.begin(9600);
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveRequest);
  Wire.onRequest(sendResponse);

  GraphicController graphicCtr(&tft, &jsonMasterDataPayload, jsonMasterTSStart);

  
  for (int i = 0; i < API_COUNT; i++) {
    apiList[i] = NULL;
  }

  NOOPController noopCtr(&tft, apiList, &graphicCtr, Serial);
  NOOPController* noopCtrPtr = &noopCtr;
  apiList[noopCtrPtr->getCtrId()] = noopCtrPtr;
  
  MainController mainCtr(&tft, apiList, &graphicCtr, Serial);
  MainController* mainCtrPtr = &mainCtr;
  apiList[mainCtrPtr->getCtrId()] = mainCtrPtr;

  SolarController solarCtr(&tft, apiList, &graphicCtr, Serial);
  SolarController* solarCtrPtr = &solarCtr;
  apiList[solarCtrPtr->getCtrId()] = solarCtrPtr;
  
  // Main Controller
  currentCtr = apiList[1];
  // NOOP Controller
  ctrToRequest = apiList[0];
    
  width = tft.width() - 1;
  height = tft.height() - 1;
  currentCtr->drawPage();
}

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
    int currCtrId = currentCtr->getCtrId();
    if (currentCtr->getRecvMasterData() && currCtrId == jsonMasterCtrId) {
      //Serial.println(currCtrId);
      //Serial.println(jsonMasterCtrId);
      currentCtr->consumeMasterData();
    }
    consuming = false;
    toclean = true;
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
    ctrToRequest = apiList[0];
    toconsume = false;
    //lock = false;
    //Serial.println("unlock ! (done parsing)");
    unsetLock("done parsing");
  }
  if (toclean) {
    // NOOP Controller
    ctrToRequest = apiList[0];
    toclean = false;
  }
}

bool doLock() {
  if (lock) {
    long currTime = millis();
    if (currTime - lockLastTime >= LOCK_TIMEOUT) {
      //Serial.println("unlock ! (timeout)");
      //lock = false;
      unsetLock("timeout");
    }
  }  
  return lock;
}

void unsetLock(String msg) {
  lock = false;
  ctrToRequest = apiList[0];
  Serial.print("unlock: ");
  Serial.println(msg);
}

void setLock() {
  lock = true;
  lockLastTime = millis();
  Serial.println("lock !"); 
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
    currTouchedPoint.x = map(currTouchedPoint.x, TS_MINX, TS_MAXX, 0, tft.width());
    currTouchedPoint.y = map(currTouchedPoint.y, TS_MINY, TS_MAXY, 0, tft.height());
    //Serial.print(currentCtr->getCtrId());
    //Serial.print(" -> ");
    int newCtrIdToRequest = currentCtr->mapUserCmd(&currTouchedPoint);
    //Serial.println(newCtrIdToRequest);
    
    APIController *newCtrToRequest = apiList[newCtrIdToRequest];   
    // Safety...
    if (newCtrToRequest == NULL) {
      newCtrToRequest = apiList[1];
    }
    
    // Too fast ?
    long interTime = millis() - lastCtrRequestTime;    
    if (ctrToRequest->getCtrId() != newCtrIdToRequest || interTime > 150) {
      ctrToRequest = newCtrToRequest;
      Serial.print("Requested: ");
      Serial.println(ctrToRequest->getCtrId());
      setLock();
      lastCtrRequestTime = millis();
    }
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
  
  // Data
  if (masterCmd == CMD_LAST_CHUNK || masterCmd == CMD_INTERMEDIATE_CHUNK) {
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
  // Commands
  }else{
    masterData = "";
    if (masterCmd == CMD_GET_TOUCH_CMD) {
      // Switching Controller
      if (ctrToRequest->getCtrId() > 0) {
        currentCtr = ctrToRequest;
        Serial.print("Switched to ");
        Serial.println(currentCtr->getCtrId());
        currentCtr->index();
        slaveResponse = String(ctrToRequest->getCtrId());
        if (currentCtr->getRecvMasterData()) {
          //lock = false;
          //Serial.println("unlock ! (sent command)");
          unsetLock("sent command");
        }
        // No default request
        ctrToRequest = apiList[0];
      }else{
        slaveResponse = String(0);
      }
      tosend = true;
    }

    if (masterCmd == CMD_GET_LAST_RECV_SIZE) {
        //Serial.println("waiting response...");
        slaveResponse = (String)masterDataSize;
        masterDataSize = 0;
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








