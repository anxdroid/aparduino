#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int recvData = 0;
int sentData = 0;
int state = 0;
int buttonState = 0;
int buttonPin = 2;
int relay1Pin = 3;
int relay1Status = 0;
int relay2Pin = 4;

// callback for received data
void receiveData(int byteCount) {

  while (Wire.available()) {
    recvData = Wire.read();
    Serial.print("data received: ");
    Serial.println(recvData);

    if (recvData == 1) {
      Serial.println("Command request !");
      toggleRelay();
      sentData = 1;
      Wire.write(sentData);
      Serial.print("data sent: ");
      Serial.println(sentData);
    }
    /*
        if (number == 1) {

          if (state == 0) {
            digitalWrite(13, HIGH); // set the LED on
            state = 1;
          }
          else {
            digitalWrite(13, LOW); // set the LED off
            state = 0;
          }
        }
    */
  }
}

void setup() {
  //pinMode(13, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);

  Serial.println("Ready!");
}

void loop() {
  delay(100);
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    Serial.println("Button pressed !");
    //toggleRelay();
    sentData = 2;
    Wire.write(sentData);
    Serial.print("data sent: ");
    Serial.println(sentData);
    delay(100);
  }

}

void toggleRelay() {
  if (relay1Status == 0) {
    digitalWrite(relay1Pin, HIGH);
    Serial.println("ON");
    relay1Status = 1;
    delay(100);
  } else {
    digitalWrite(relay1Pin, LOW);
    Serial.println("OFF");
    relay1Status = 0;
    delay(100);
  }
}


