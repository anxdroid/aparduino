/*
  Measuring Current Using ACS712
*/
const int analogIn = A1;
const int photoRes = A2;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
int RawValue = 0;
int ACSoffset = 2500;
double Voltage = 0;
double Amps = 0;

void setup() {
  Serial.begin(9600);
  pinMode(analogIn, INPUT);
  pinMode(photoRes, INPUT);
}

void loop() {
  float currTime = millis();
  float lastTime = millis();
  double Current = 0;
  do {
    RawValue = abs(analogRead(analogIn));
    Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
    Amps = ((Voltage - ACSoffset) / mVperAmp);
    if (Amps > Current) {
      Current = Amps;
    }
    currTime = millis();
  } while ((currTime - lastTime) <= 20);
  Current /= sqrt(2);

  int lightval = 1023 - analogRead(photoRes);

  //Serial.print("Raw Value = " ); // shows pre-scaled value
  //Serial.println(RawValue);
  unsigned long time = millis();
  Serial.print("MILLIS:");
  Serial.print(time);
  Serial.print(":msec");

  Serial.print(" ");

  Serial.print("40:CURRENT_TERMO:");
  Serial.print(Current);
  Serial.print(":A");

  Serial.print(" ");

  Serial.print("40:LIGHT_TERRAZZO:");
  Serial.print(lightval);
  Serial.print(":&perc;");

  Serial.println();


  //Serial.print("Light level: ");
  //Serial.println(lightval);

  delay(10000);

}
