/* Misura la temperatura con sensore DS18B20
   collegato al piedino A0
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A0
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);

void setup() 
{
delay(1000);
Serial.begin(9600);
Serial.println("Misura la temperatura con sensore DS18B20");
Serial.println("CFP Manfredini Este PD");
delay(1000);
sensors.begin();
}

void loop() 
{
Serial.println();
sensors.requestTemperatures();
//Serial.print("Temperatura = ");
Serial.print(sensors.getTempCByIndex(0));
//Serial.println(" gradi C");
}
