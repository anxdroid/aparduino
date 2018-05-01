#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "apmain";
const char* password = "montefalconedivalfortore2014";
String cmdTokens[4];
char delimiter = ';';
String apikey = "a7441c2c34fc80b6667fdb1717d1606f";

//IPAddress host(192,168,1,12);
String hostEmonCms = "192.168.1.12";                                 
int portEmonCms = 80;
WiFiClient clientEmonCms;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 1, 21);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  //Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //Serial.print(".");
  }
}

int tokenize(String cmd) {
  String tmpToken = "";
  int currToken = 0;
  for (int i = 0; i < cmd.length(); i++) {
    if (cmd.charAt(i) != delimiter) {
      tmpToken += cmd.charAt(i);
    }else{
     cmdTokens[currToken] = tmpToken;
     tmpToken = "";
     currToken++;
    }
  }

  if (tmpToken != "") {
    cmdTokens[currToken] = tmpToken;
    tmpToken = "";
    currToken++;
  }
  return currToken;
}

long doLog(long start) {
  long getTime = millis();
  long delta = getTime - start;
  Serial.println(delta);
  return getTime;
}

String emonCmsFeed(int feedId) {
  String url = "/emoncms/feed/timevalue.json?id=";
  url += feedId;
  url += "&apikey=";
  url += apikey;
  
  //Serial.println("---------------------------");
  clientEmonCms.setTimeout(1000);
  while (!clientEmonCms.connect(hostEmonCms.c_str(), portEmonCms));  
  long start = millis();
  
  String httpCall = "GET " + url + " HTTP/1.1\r\nHost: "+hostEmonCms+":"+String(portEmonCms)+"\r\nConnection: close\r\n\r\n";
  //Serial.println(httpCall);
  clientEmonCms.print(httpCall);
  
  //start = doLog(start);
  while (clientEmonCms.available() == 0);
  
  //start = doLog(start);
  while (clientEmonCms.available()) {
    String line = clientEmonCms.readStringUntil('\r');
    //start = doLog(start);
    //Serial.println(line);
    if (line == "\n") {
      break;
    }
  }  
  String line = clientEmonCms.readStringUntil('\r');
  clientEmonCms.stop();
  return line.substring(1);
}

String parseCmd(String cmd) {
  int tokens = tokenize(cmd);
  if (tokens > 0) {
    String res = cmd;
    res += ": Unknown Command";
    if (cmdTokens[0] == "emonCmsFeed") {
      if (WiFi.status() == WL_CONNECTED) {
        char buffer[4];
        cmdTokens[1].toCharArray(buffer, 4);
        int feedId = atoi(buffer);
        res = emonCmsFeed(feedId);
      }
      //Serial.println("---------------------------");
      //Serial.println(res);
    }else if(cmdTokens[0] == "getWiFiStatus") {
      res = "Not Connected";
      if (WiFi.status() == WL_CONNECTED) {
        res = WiFi.localIP().toString();
      }
    }
    return res;
  }
}

void loop(void) {

  String cmd = "getWiFiStatus";
  String res = parseCmd(cmd);
  Serial.println(res);
  cmd = "emonCmsFeed;10";
  res = parseCmd(cmd);
  Serial.println(res);
  delay(2000);
  
  if (Serial.available() > 0) {
    String cmd = Serial.readString();
    String res = parseCmd(cmd);
    Serial.println(res);
  }
}

