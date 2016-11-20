const int currentPin = 1;
const int CYCLES = 10;

float lastCycle = 0;

unsigned int frequency[CYCLES];
float maxVal[CYCLES];
float minVal[CYCLES];

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int i = 0;
  float maxCycleVal = 0;
  float minCycleVal = -100;
  while (i < CYCLES) {
    float currTime = millis();
    long diff = currTime-lastCycle;
    
    float currVal = abs((analogRead(currentPin) - 512) * (30000/461)) - 130;

    if (currVal > maxCycleVal) {
      maxCycleVal = currVal;
    }
    if (currVal < minCycleVal || minCycleVal == -100) {
      minCycleVal = currVal;
    }    
    
    if (lastCycle == 0 || (diff >= 20)) {
      float hz = 1000/diff;
      /*
      Serial.print("New cycle at ");
      Serial.print(lastCycle);
      Serial.print(" diff ");
      Serial.print(diff);
      Serial.println("musecs");
      Serial.print(" ");
      Serial.print(hz);
      Serial.println("hz");
      */
      frequency[i] = hz;
      maxVal[i] = maxCycleVal;
      double eff = maxCycleVal / sqrt(2);
      Serial.println(eff);
      minVal[i] = minCycleVal;
      
  
      lastCycle = currTime;
      i++;
      int maxCycleVal = 0;
      int minCycleVal = -100;
    }
  }

  //for (int i = 0; i<CYCLES; i++) {
    /*
    Serial.print("New cycle at ");
    Serial.print(frequency[i]);
    Serial.print("hz current ");
    Serial.print(maxVal[i] / 1000);
    */
    //double eff = maxVal[i] / sqrt(2);
    //Serial.print("A eff ");

    //Serial.println(eff);
    //Serial.print("mA");
     
  //}
  //Serial.println(currTime);
  //unsigned int adcZero = 512;                                                     // relative digital zero of the arudino input from ACS712 (could make this a variable and auto-adjust it)
  //int currVal = analogRead(currentPin);
  //currVal -= adcZero;
  //float intensity = currVal * (30000/461);
  //if (maxVal < intensity) {
  //  maxVal = intensity;
  //}
  //intensity = maxVal / sqrt(2);
  //Serial.println(intensity);

}

