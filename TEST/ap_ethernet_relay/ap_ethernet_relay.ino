#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(79, 22, 220, 117); // numeric IP for Google (no DNS)
IPAddress server(192, 168, 1, 3); // numeric IP for Google (no DNS)
//char server[] = "http://192.168.1.3:80/temp/jobs.php";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress mydns(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

String id = "0";
String cmd = "";
String param = "";

String prev_id = "0";
String prev_cmd = "";
String prev_param = "";
OneWire  ds(6);
DallasTemperature sensors(&ds);


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  sensors.begin();
  pinMode(10, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(10, HIGH);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Starting.....");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, mydns, gateway, subnet);
  } else {
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(5000);


}


int parseCmd() {
  if (cmd == "HEATERS") {
    if (param == "ON") {
      Serial.println("Accendo termosifoni");
      digitalWrite(7, HIGH);
    } else {
      Serial.println("Spengo termosifoni");
      digitalWrite(7, LOW);
    }
  }
  return 0;
}

int parseClient() {
  int countLF = 0;
  bool payload = false;
  int n_token = 0;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\r') {
        countLF++;
      } else {
        if (c == '\n') {
          countLF += 10;
        } else {
          countLF = 0;
        }
      }
      if (countLF > 20) {
        payload = true;
        continue;
      }
      if (payload) {
        //Serial.print(countLF);
        //Serial.print(c);

        if (c == ' ') {
          n_token++;
        } else if (c == ':') {
          n_token++;
        } else if (c == '\r' || c == '\n') {
          n_token++;
        } else {
          if (n_token == 0) {
            id += c;
          } else if (n_token == 1) {
            cmd += c;
          } else if (n_token == 2) {
            param += c;
          }
        }


      }
      //if (c == " ") {
      //  Serial.println();
      //}
    }
  }
  if (payload) {
    return 0;
  } else {
    return 1;
  }
}

void loop()
{
  sensors.requestTemperatures();  
  
  //Serial.print("connecting...");
  if (client.connect(server, 80)) {
    //Serial.println("connected");
  }
  // if you get a connection, report back via serial:
  if (client.connected()) {


    // ***************** Invio di dati acquisiti
    
    float temp = sensors.getTempCByIndex(1);
    
    // Make your API request:
    if (temp >= -5 && temp <= 45) {
      Serial.print("Sending TEMP_SOTTOTETTO=");
      Serial.println(temp);
      client.print("GET /temp/measure_mysql.php?sensor=TEMP_SOTTOTETTO&value=");
      
      client.print(temp);
      client.println("&unit=%26deg%3B HTTP/1.1");
      client.println("Host: 192.168.1.3:80");
      client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
      client.println("Connection: close");
      client.println();
      client.stop();
      Serial.println("...sent !");

      if (client.connect(server, 80)) {
        //Serial.println("connected");
      }
    }


    temp = sensors.getTempCByIndex(2);
    if (temp >= -5 && temp <= 45) {
      Serial.print("Sending TEMP_SALOTTO=");
      Serial.println(temp);
      // Make your API request:
      client.print("GET /temp/measure_mysql.php?sensor=TEMP_SALOTTO&value=");
      
      client.print(temp);
      client.println("&unit=%26deg%3B HTTP/1.1");
      client.println("Host: 192.168.1.3:80");
      client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
      client.println("Connection: close");
      client.println();
      client.stop();
      Serial.println("...sent !");    
  
      //Serial.print("connecting...");
      if (client.connect(server, 80)) {
        //Serial.println("connected");
      }
    }

    temp = sensors.getTempCByIndex(0);
    if (temp >= -5 && temp <= 45) {
      Serial.print("Sending TEMP_DISIMPEGNO=");
      Serial.println(temp);
      // Make your API request:
      client.print("GET /temp/measure_mysql.php?sensor=TEMP_DISIMPEGNO&value=");
      
      client.print(temp);
      client.println("&unit=%26deg%3B HTTP/1.1");
      client.println("Host: 192.168.1.3:80");
      client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
      client.println("Connection: close");
      client.println();
      client.stop();
      Serial.println("...sent !");    
  
      //Serial.print("connecting...");
      if (client.connect(server, 80)) {
        //Serial.println("connected");
      }
    }    

      // ***************** Richiesta di comandi

    
    client.println("GET /temp/jobs.php?req_cmd=HEATERS&simple_out=1 HTTP/1.1");
    client.println("Host: 192.168.1.3:80");
    client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
    client.println("Connection: close");
    client.println();
    int retval = parseClient();
    client.stop();

    if (retval == 0) {
      Serial.println("--------");
      Serial.println(id);
      Serial.println(cmd);
      /*
        if (param == prev_param) {
        if (param == "ON") {
          param = "OFF";
        }else{
          param = "ON";
        }
        }
      */
      Serial.println(param);
      int retcmd = parseCmd();
      if (retcmd == 0) {
        //Serial.print("connecting...");
        if (client.connect(server, 80)) {
          //Serial.println("connected");
        }
        client.print("GET /temp/jobs.php?job_id=");
        client.print(id);
        client.println(" HTTP/1.1");
        client.println("Host: 192.168.1.3:80");
        client.println("Authorization: Basic YW50bzpyZXNpc3RvcmU=");
        client.println("Connection: close");
        client.println();

        client.stop();
      }
      prev_id = id;
      prev_cmd = cmd;
      prev_param = param;
      id = cmd = param = "";
    }

  } else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  delay(5000);
  /*
    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();

      // do nothing forevermore:
      while (true);
    }
  */
}
