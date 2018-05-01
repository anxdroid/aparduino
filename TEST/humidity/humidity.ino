#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int t = dht.readTemperature();
  int h = dht.readHumidity();
  //Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" ");
  if (t >= 24) {
    //digitalWrite(6, LOW);
  }else{
    //digitalWrite(6, HIGH);
  }
  //Serial.print("Humidity: ");
  Serial.println(h);
  if (h <= 50) {
    //digitalWrite(7, LOW);
  }else{
    //digitalWrite(7, HIGH);
  }  
  delay(30000);
}
