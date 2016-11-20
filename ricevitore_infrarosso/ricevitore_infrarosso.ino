// load the library
#include <IRremote.h>

// define where theoutput is connected
const int RECV_PIN = 2;

// define a receiver object
IRrecv irrecv(RECV_PIN);

// define there the results should goto
decode_results results;

void setup()
{
  // start the serial output
  Serial.begin(115200);
  // start the receiver
  irrecv.enableIRIn(); 
}

void loop() 
{
  // if there is a result
  if (irrecv.decode(&results)) 
  {
    // print it to the serial line
    Serial.println(results.value, HEX);
    // receive the next value
    irrecv.resume(); 
  }
}
