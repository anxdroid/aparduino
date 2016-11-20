#define DEBUGIRMS true

#include <SPI.h>
#include <EmonLib.h>

/* DEFINITIONS */
#define BUFSIZE 255

int digitalPinRed = 9;

EnergyMonitor emon1;          // Create an instance

float IConsumata = 0;
//float IProdotta = 0;
float WConsumata = 0;
//float WProdotta = 0;

/* PROGRAMM */
void setup() {
  //  turn on serial (for debugging)
  Serial.begin(9600);
  //emon1.current(1, 66);             // Current: input pin, calibration.
  //emon2.current(2, 60);             // Current: input pin, calibration.
  emon1.voltage(0, 217, 1.7);  // Voltage: input pin,                                 // calibration, phase_shift
  //emon2.voltage(0, 234.26, 1.7);  // Voltage: input pin,
  // calibration, phase_shift
  emon1.current(1, 66);       // Current: input pin, calibration.
  //emon2.current(2, 60);       // Current: input pin, calibration.

}

void loop() {
  emon1.calcVI(20, 2000);   // Calculate all. No.of wavelengths, time-out
  //emon2.calcVI(20, 2000);   // Calculate all. No.of wavelengths, time-out


  double Irms = emon1.calcIrms(1480); // Calculate Irms only
  //double Vrms = emon1.calcVrms(1480); // Calculate Vrms only
  //Serial.print(Irms); // Irms
  //Serial.print(","); //
  //Serial.println(Vrms); // Vrms


  IConsumata = emon1.Irms;
  //IProdotta = emon2.Irms;
  WConsumata = emon1.apparentPower;
  //WProdotta = emon2.apparentPower;


  //IProdotta = emon2.calcIrms(1480);  // Calculate Irms only
  // Se corrente Prelevata superiore quella prodotta accendo il led
  /*
  pinMode(digitalPinRed, OUTPUT);
  if  (IConsumata > IProdotta) {
    digitalWrite(digitalPinRed, HIGH);
  }
  else {
    digitalWrite(digitalPinRed, LOW);
  }
  delay(10);
*/
#if DEBUGIRMS
  //Serial.print(getTimeDisplay());
  Serial.print(" - Watt Prelevata= ");
  Serial.print(WConsumata);
  Serial.print(" (");
  Serial.print(IConsumata);
  /*
  Serial.print("  A) - Watt Prodotta= ");
  Serial.print(WProdotta);
  Serial.print(" (");
  Serial.print(IProdotta);
  */
  Serial.println(" A)");
  
#endif
}
