/*
  EmonTx CT123 Voltage Serial Only example
  
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3
  
  Author: Trystan Lea
*/

#include "EmonLib.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Create  instances for each CT channel
EnergyMonitor ct1,ct2,ct3, ct4;

// On-board emonTx LED
const int LED1 = 1;                                                    
const int LED0 = 0;

                                                    // Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//DeviceAddress address_T1 = { 0x28, 0x22, 0x70, 0xEE, 0x02, 0x00, 0x00, 0xB8 };
DeviceAddress address_T1 = { 0x28, 0xFF, 0x96, 0xAB, 0x74, 0x16, 0x03, 0x8B };

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

  delay(1000);
  Serial.begin(9600);
  Serial.println("Temperature search");
  Serial.println("waiting 6 seconds before printing");
  delay(6000);

  sensors.begin();
  
  DeviceAddress tmp_address;
  int numberOfDevices = sensors.getDeviceCount();
  
  for(int i=0;i<numberOfDevices; i++)
  {
    sensors.getAddress(tmp_address, i);
    printAddress(tmp_address);
    Serial.println();
  }
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

  Serial.print("10:POWER_SOLAR:");
  Serial.print(ct3.realPower);
  Serial.print(":W");
  
  Serial.print(" ");

  Serial.print("10:CURRENT_SOLAR:");
  Serial.print(ct3.Irms);
  Serial.print(":A");
  
  Serial.print(" "); 
  //Serial.print(ct4.realPower);
  //Serial.print(" ");

  Serial.print("10:VOLTAGE:");
  Serial.print(ct3.Vrms);
  Serial.print(":V");

  Serial.print(" "); 

  Serial.print("10:TEMP_TERRAZZO:");
  sensors.requestTemperatures();
  double temperature = sensors.getTempC(address_T1);  // Get the temperature of the sensor
  Serial.print(temperature); 
  Serial.print(":&deg;");
           
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

void printAddress(DeviceAddress deviceAddress)
{
  Serial.print("{ ");
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    Serial.print("0x");
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i<7) Serial.print(", ");
    
  }
  Serial.print(" }");
}
