void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("starting...");
  if (Serial2.available()) {
    Serial.println("garbage received !");
    String recv = Serial2.readString();
    Serial.println(recv);
  }
}

void loop() {
  Serial.println("looping...");
  // put your main code here, to run repeatedly:
  if (Serial2.available()) {
    String recv = Serial2.readString();
    Serial.println("msg received !");
    Serial.println(recv);
  }
  delay(2000);
}
