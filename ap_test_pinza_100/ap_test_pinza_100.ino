/*
  EmonTx CT123 Voltage Serial Only example
  
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3
  
  Author: Trystan Lea
*/

#include "EmonLib.h"

// Create  instances for each CT channel
EnergyMonitor ct1,ct2,ct3, ct4;

// On-board emonTx LED
const int LED1 = 1;                                                    
const int LED0 = 0;                                                    

void setup() 
{
  Serial.begin(9600);
  // while (!Serial) {}
  // wait for serial port to connect. Needed for Leonardo only
  
  Serial.println("emonTX Shield CT123 Voltage Serial Only example"); 
  Serial.println("OpenEnergyMonitor.org");
  
  // Calibration factor = CT ratio / burden resistance = (100A / 0.05A) / 33 Ohms = 60.606
  //ct1.current(1, 60.606);
  //ct2.current(2, 60.606);                                     
  ct3.current(3, 60.606);
  //ct4.current(4, 60.606); 
  
  // (ADC input, calibration, phase_shift)
  //ct1.voltage(0, 300.6, 1.7);                                
  //ct2.voltage(0, 300.6, 1.7);                                
  ct3.voltage(0, 242, 1.4);
  //ct4.voltage(0, 300.6, 1.7);
  
  // Setup indicator LED
  //pinMode(LED1, OUTPUT);
  //pinMode(LED0, OUTPUT);                                                        
}

void loop() 
{ 
  // Calculate all. No.of crossings, time-out 
  //ct1.calcVI(20,2000);                                                  
  //ct2.calcVI(20,2000);
  ct3.calcVI(20,2000);
  //ct4.calcVI(20,2000);
  unsigned long time = millis();  
  // Print power 
  //Serial.print(ct1.realPower);     
  //Serial.print(" "); 
  //Serial.print(ct2.realPower);
  //Serial.print(" "); 
  Serial.print("MILLIS:");
  Serial.print(time);
  Serial.print(":msec");

  Serial.print(" ");

  Serial.print("POWER_SOLAR:");
  Serial.print(ct3.realPower);
  Serial.print(":W");
  
  Serial.print(" ");

  Serial.print("CURRENT_SOLAR:");
  Serial.print(ct3.Irms);
  Serial.print(":A");
  
  Serial.print(" "); 
  //Serial.print(ct4.realPower);
  //Serial.print(" ");

  Serial.print("VOLTAGE:");
  Serial.print(ct3.Vrms);
  Serial.print(":V");

  Serial.println();
  
    
  // Available properties: ct1.realPower, ct1.apparentPower, ct1.powerFactor, ct1.Irms and ct1.Vrms

  /*
  digitalWrite(LED0, HIGH);
  delay(100);
  digitalWrite(LED1, HIGH);
  delay(1000);
  digitalWrite(LED0, LOW);
  delay(100);
  digitalWrite(LED1, LOW);
  */
  
  delay(5000);
}
