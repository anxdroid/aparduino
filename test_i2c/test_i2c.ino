#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int button1Pin = 12;
int button2Pin = 11;

int relay1Pin = 3;
int relay1Status = LOW;
int relay2Pin = 4;
int relay2Status = LOW;

int led1Pin = 8;
int led2Pin = 9;

int recvData = 0;
int sentData = 0;

void toggleStatus(int nRelay, int val) {
  int relayPin = relay1Pin;
  int relayStatus = relay1Status;
  int ledPin = led1Pin;
  if (nRelay == 2) {
    relayPin = relay2Pin;
    relayStatus = relay2Status;
    ledPin = led2Pin;
  }
  if (val == HIGH) {
    if (relayStatus == HIGH) {
      relayStatus = LOW;
    } else {
      relayStatus = HIGH;
    }
    Serial.print("Button ");
    Serial.print(nRelay);
    Serial.print(" pressed status ");
    Serial.println(relayStatus);
    digitalWrite(relayPin, relayStatus);
    digitalWrite(ledPin, relayStatus);

    if (nRelay == 1) {
      relay1Status = relayStatus;
    }else if (nRelay == 2) {
      relay2Status = relayStatus;
    }
    delay(300);
  }
  
}

// callback for received data
void receiveData(int byteCount) {

  while (Wire.available()) {
    recvData = Wire.read();
    Serial.print("data received: ");
    Serial.println(recvData);

    if (recvData > 0 && recvData < 3) {
      Serial.println("Command request !");
      toggleStatus(recvData, HIGH);
      sentData = 1;
      Wire.write(sentData);
      Serial.print("data sent: ");
      Serial.println(sentData);
    }
  }
}

void setup() {
  Serial.begin(9600); // start serial for output
  Serial.println("Start... ");
  // put your setup code here, to run once:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  
  Wire.begin(SLAVE_ADDRESS);
  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Serial.println("Ready!");
}

void loop() {
  delay(100);
  int val1 = digitalRead(button1Pin);
  toggleStatus(1, val1);
  int val2 = digitalRead(button2Pin);
  toggleStatus(2, val2);
}


