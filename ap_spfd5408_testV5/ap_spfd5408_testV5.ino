#include "touchTest.h"

void setup(void) {
  width = tft.width() - 1;
  height = tft.height() - 1;
  
  Serial3.begin(115200);
  Serial.begin(115200);

  tft.reset();
  tft.begin(0x9341); // SDFP5408
  tft.setRotation(3);

  for (int i = 0; i < API_COUNT; i++) {
    ctrList[i] = NULL;
  }
  
  registerCtr(&solarCtr);
  solarCtr.setModel(&emonCMSModel);
  
  registerCtr(&tempCtr);
  tempCtr.setModel(&emonCMSModel);
  
  registerCtr(&settingsCtr);
  settingsCtr.setModel(&emonCMSModel);

  registerCtr(&backgroundCtr);
  settingsCtr.setModel(&emonCMSModel);
  
  registerCtr(&mainCtr);
  mainCtr.setModel(&emonCMSModel);


  //Serial.println("-------------------------------------");

  // Switch to Main Controller
  currentCtr = getCtrByName("MainController2");
  //tft.fillScreen(BLACK);
  currentCtr->index();
  ctrToRequest = NULL;
}

void getUserCmd() {
  if (doLock()) {
    return;
  }
  gettingusercmd = true;
  digitalWrite(13, HIGH);
  currTouchedPoint.z = 0;
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(YP, OUTPUT);     //.kbv these pins are shared with TFT
  pinMode(XM, OUTPUT);     //.kbv these pins are shared with TFT
  //Serial.println(ctrToRequest->getCtrId());

  if (p.z > 0) {
    //Serial.println(currentCtr->getCtrName());
    /*
    Serial.print(p.x);
    Serial.print(",");
    Serial.println(p.y); 
    */

    currTouchedPoint.x = map(p.y, TS_MINY, TS_MAXY, tft.width(), 0);
    currTouchedPoint.y = map(p.x, TS_MAXX, TS_MINX, tft.height(), 0);   
    
    int res = currentCtr->mapUserCmd(&currTouchedPoint, newCtrNameToRequestBuf);
    /*
    Serial.print("Requested ");
    Serial.println(newCtrNameToRequestBuf);
    */

    // Calling new controller or just action ?
    if (res == 1) {
      APIController2 *newCtrToRequest = getCtrByName(newCtrNameToRequestBuf);
      
      // Too fast ?
      long interTime = millis() - lastCtrRequestTime;
      char ctrNameBuf[CTRNAMESIZE];
      currentCtr->getCtrName(ctrNameBuf);    
      if (
            (newCtrToRequest != NULL) &&
            (
              (strcmp(ctrNameBuf, newCtrNameToRequestBuf) != 0) || 
              (interTime > 150)
            )
         ) {
        ctrToRequest = newCtrToRequest;
        //Serial.print("Requested: ");
        //Serial.println(ctrToRequest->getCtrName());
        setLock();
        lastCtrRequestTime = millis();
      }
    }
  }
  gettingusercmd = false;
}

void loop(void) {
  ctrToRequest = NULL;
  
  // Getting User Command
  getUserCmd();

  // Switching controller
  if (ctrToRequest != NULL) {
    // Destroy all dynamic objects
    currentCtr->cleanUIElements();
    // Set current Controller
    currentCtr = ctrToRequest;
    
    char ctrNameBuf[CTRNAMESIZE];
    currentCtr->getCtrName(ctrNameBuf);
    
    //Serial.print("Switched to ");
    //Serial.println(ctrNameBuf);
    
    // Launch main controller action
    //tft.fillScreen(BLACK);
    currentCtr->index();
    
    // Ready for next step...
    ctrToRequest = NULL;
    unsetLock("");
  }

  // Background controllers
  for (int ctrId = 0; ctrId < API_COUNT; ctrId++) {
    APIController2* ctrPtr = ctrList[ctrId];
    if (ctrPtr != NULL && ctrPtr->isInBackground()) {
      ctrPtr->background();
    }
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
