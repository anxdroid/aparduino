void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  Serial1.print('H');
  delay(1000);
  Serial1.print('L');
  delay(1000);
}
