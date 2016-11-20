#define RELAY1  13
#define RELAY2  8
#define RELAY3  7
#define RELAY4  12
int LDR = 0;     //analog pin to which LDR is connected, here we set it to 0 so it means A0
int LDRValue = 0;      //thatâ€™s a variable to store LDR values
int threshold = 400;


int relay1_status = 0;
int relay2_status = 0;
int relay3_status = 0;
int relay4_status = 0;


void setup()
{
  Serial.begin(9600);          //start the serial monitor with 9600 buad
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);
}

void loop()
{
  LDRValue = analogRead(LDR);      //reads the ldrâ€™s value through LDR
  if (LDRValue < (1.1 * threshold) && relay1_status == 0) {
    digitalWrite(RELAY1, LOW);
    delay(200);
    digitalWrite(RELAY2, LOW);
    delay(200);
    digitalWrite(RELAY3, LOW);
    delay(200);
    digitalWrite(RELAY4, LOW);
    relay1_status = 1;
    //Serial.println("relay on");
  }
  if (LDRValue > (1.1 * threshold) && relay1_status == 1) {
    digitalWrite(RELAY1, HIGH);
    delay(200);
    digitalWrite(RELAY2, HIGH);
    delay(200);
    digitalWrite(RELAY3, HIGH);
    delay(200);
    digitalWrite(RELAY4, HIGH);
    relay1_status = 0;
    //Serial.println("relay off");
  }
  Serial.println(100000*LDRValue+relay1_status);       //prints the LDR values to serial monitor
  //Serial.print(" ");
  //Serial.println(relay1_status);
  
  delay(500);        //This is the speed by which LDR sends value to arduino
}

