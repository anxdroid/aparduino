#include "EmonLib.h"                   // Include Emon Library:  https://github.com/openenergymonitor/EmonLib
EnergyMonitor ct1;                     // Create an instance
const int LEDpin = 9;

void setup()
{  
  Serial.begin(9600);
  //ct1.currentTX(1, 111.1);             // CT channel (see emontx PCB), calibration (2000 turns / 18 Ohm burden resistor = 111.1)
  ct1.voltage(0, 222, 1.4);
  //ct1.voltageTX(228, 1.7);
  ct1.current(1, 30);
}

void loop()
{
  ct1.calcVI(20,2000);    // Calculate all. No.of wavelengths, time-out
  double Irms = ct1.calcIrms(1480);    // Calculate RMS current (1480: no. of samples)
  double realpower1 = ct1.realPower;
  double apparentpower1 = ct1.apparentPower;

  //double Vrms = ct1.calcVrms(1480); // Calculate Vrms only 
  Serial.print(Irms); // Irms 
  Serial.print(" ");
  Serial.print(realpower1); // Irms 
  Serial.print(" ");
  Serial.println(apparentpower1); // Irms 
  //Serial.print(","); // 
  //Serial.println(Vrms); // Vrms
}
