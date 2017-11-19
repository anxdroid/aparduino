#include <Wire.h>

int button1Pin = 12;
int button2Pin = 11;

int relay1Pin = 3;
int relay1Status = LOW;
int relay2Pin = 4;
int relay2Status = LOW;

int led1Pin = 8;
int led2Pin = 9;

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
}
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

void loop() {
  // put your main code here, to run repeatedly:
  int val1 = digitalRead(button1Pin);
  /*
  if (val1 == HIGH) {
    if (relay1Status == HIGH) {
      relay1Status = LOW;
    } else {
      relay1Status = HIGH;
    }
    Serial.print("Button 1 pressed status ");
    Serial.println(relay1Status);
    digitalWrite(relay1Pin, relay1Status);
    digitalWrite(led1Pin, relay1Status);
    delay(300);
  }
  */
  toggleStatus(1, val1);
  
  int val2 = digitalRead(button2Pin);
  /*
  if (val2 == HIGH) {
    if (relay2Status == HIGH) {
      relay2Status = LOW;
    } else {
      relay2Status = HIGH;
    }
    Serial.print("Button 2 pressed status ");
    Serial.println(relay2Status);
    digitalWrite(relay2Pin, relay2Status);
    digitalWrite(led2Pin, relay2Status);
    delay(300);
  }
  */
  toggleStatus(2, val2);

}
