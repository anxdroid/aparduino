#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
     
// Ð²Ð¿Ð¸ÑˆÐ¸Ñ‚Ðµ Ñ�ÑŽÐ´Ð° Ð´Ð°Ð½Ð½Ñ‹Ðµ, Ñ�Ð¾Ð¾Ñ‚Ð²ÐµÑ‚Ñ�Ñ‚Ð²ÑƒÑŽÑ‰Ð¸Ðµ Ð²Ð°ÑˆÐµÐ¹ Ñ�ÐµÑ‚Ð¸:
const char* ssid = "apmain";
const char* password = "montefalconedivalfortore2014";

ESP8266WebServer server(80);
MDNSResponder mdns;
     
String webPage = "";
     
int led_pin = 13;
     
void setup(void){
     
  // Ð¿Ð¾Ð´Ð³Ð¾Ñ‚Ð¾Ð²ÐºÐ°:
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸Ñ� Ð¾ ÐºÐ¾Ð½Ñ‚Ñ€Ð¾Ð»Ð»ÐµÑ€Ðµ
  Serial.println("");
  Serial.println("ESP8266 board info:");
  Serial.print("\tChip ID: ");
  Serial.println(ESP.getFlashChipId());
  Serial.print("\tCore Version: ");
  Serial.println(ESP.getCoreVersion());
  Serial.print("\tChip Real Size: ");
  Serial.println(ESP.getFlashChipRealSize());
  Serial.print("\tChip Flash Size: ");
  Serial.println(ESP.getFlashChipSize());
  Serial.print("\tChip Flash Speed: ");
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print("\tChip Speed: ");
  Serial.println(ESP.getCpuFreqMHz());
  Serial.print("\tChip Mode: ");
  Serial.println(ESP.getFlashChipMode());
  Serial.print("\tSketch Size: ");
  Serial.println(ESP.getSketchSize());
  Serial.print("\tSketch Free Space: ");
  Serial.println(ESP.getFreeSketchSpace());

  // Ñ‚ÐµÐ»Ð¾ Ð²ÐµÐ±-Ñ�Ñ‚Ñ€Ð°Ð½Ð¸Ñ†Ñ‹
  webPage += "<h1>ESP8266 Web Server</h1>";
  webPage += "<p>Chip ID: ";
  webPage += ESP.getFlashChipId();
  webPage += "</p>";
  webPage += "<p>Core Version: ";
  webPage += ESP.getCoreVersion();
  webPage += "</p>";
  webPage += "<p>Chip Real Size: ";
  webPage += ESP.getFlashChipRealSize()/1024;
  webPage += " Kbit</p>";
  webPage += "<p>Chip Size: ";
  webPage += ESP.getFlashChipSize()/1024;
  webPage += " Kbit</p>";
  webPage += "<p>Chip Flash Speed: ";
  webPage += ESP.getFlashChipSpeed()/1000000;
  webPage += " MHz</p>";
  webPage += "<p>Chip Work Speed: ";
  webPage += ESP.getCpuFreqMHz();
  webPage += " MHz</p>";
  webPage += "<p>Chip Mode: ";
  webPage += ESP.getFlashChipMode();
  webPage += "</p>";
  webPage += "<p>LED state <a href=\"LedON\"><button>ON</button></a>&nbsp;<a href=\"LedOFF\"><button>OFF</button></a></p>";

  // Ð¿Ð¾Ð´ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ðµ Ðº WiFi
  WiFi.begin(ssid, password);
  Serial.println("");
     
  // Ð¾Ð¶Ð¸Ð´Ð°Ð½Ð¸Ðµ Ñ�Ð¾ÐµÐ´Ð¸Ð½ÐµÐ½Ð¸Ñ�:
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");  //  "ÐŸÐ¾Ð´ÐºÐ»ÑŽÑ‡Ð¸Ð»Ð¸Ñ�ÑŒ Ðº "
  Serial.println(ssid);
  Serial.print("IP address: ");  //  "IP-Ð°Ð´Ñ€ÐµÑ�: "
  Serial.println(WiFi.localIP());

  //  ÐŸÑ€Ð¾Ð²ÐµÑ€ÐºÐ° Ð·Ð°Ð¿ÑƒÑ�ÐºÐ° MDNS
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
     
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  
  server.on("/LedON", [](){
    server.send(200, "text/html", webPage);
    digitalWrite(led_pin, HIGH);
    Serial.println("[ON]");
    delay(1000);
  });
  
  server.on("/LedOFF", [](){
    server.send(200, "text/html", webPage);
    digitalWrite(led_pin, LOW);
    Serial.println("[OFF]");
    delay(1000);
  });
  
  server.begin();
  Serial.println("HTTP server started");
  
}
     
void loop(void){
  server.handleClient();
}

