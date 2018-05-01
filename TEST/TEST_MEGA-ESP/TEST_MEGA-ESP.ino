#include <MemoryFree.h>
#include <EEPROM.h>

#define PIN_LED 13    // Ð²Ñ‹Ð²Ð¾Ð´ Ñ�Ð²ÐµÑ‚Ð¾Ð´Ð¸Ð¾Ð´Ð°
String inString;

// Ð�Ð°Ñ�Ñ‚Ñ€Ð¾Ð¹ÐºÐ°
void setup() {
  // Ð˜Ð½Ð¸Ñ†Ð¸Ð°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ� Ð¿Ð¾Ñ€Ñ‚Ð¾Ð² Ð¸ Ð²Ñ‹Ñ…Ð¾Ð´Ð¾Ð²
  Serial.begin(115200);
  Serial3.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

// Ð’Ñ‹Ð¿Ð¾Ð»Ð½ÐµÐ½Ð¸Ðµ
void loop() {
}

// ÐŸÑ€Ð¾Ð²ÐµÑ€ÐºÐ° Ñ�Ð¾Ð±Ñ‹Ñ‚Ð¸Ñ� Ð½Ð° Ð¿Ð¾Ñ€Ñ‚Ñƒ Serial3
void serialEvent3() {
  while (Serial3.available()) {
    // Ð§Ñ‚ÐµÐ½Ð¸Ðµ Ð´Ð°Ð½Ð½Ñ‹Ñ… Ð¸Ð· Ð¿Ð¾Ñ€Ñ‚Ð° Serial3
    char inChar = Serial3.read();
    // Ð’Ñ‹Ð²Ð¾Ð´ Ð¿Ñ€Ð¾Ñ‡Ð¸Ñ‚Ð°Ð½Ð½Ñ‹Ñ… Ð´Ð°Ð½Ð½Ñ‹Ñ… Ð² Ð¿Ð¾Ñ€Ñ‚ Serial
    Serial.write(inChar);
    // ÐŸÐ¾Ð¸Ñ�Ðº ÐºÐ¾Ð¼Ð°Ð½Ð´Ñ‹ Ð² Ð¿Ð¾Ð»ÑƒÑ‡ÐµÐ½Ð½Ñ‹Ñ… Ð´Ð°Ð½Ð½Ñ‹Ñ… (ÐºÐ¾Ð¼Ð°Ð½Ð´Ð° Ð´Ð¾Ð»Ð¶Ð½Ð° Ð±Ñ‹Ñ‚ÑŒ Ð² ÐºÐ²Ð°Ð´Ñ€Ð°Ñ‚Ð½Ñ‹Ñ… Ñ�ÐºÐ¾Ð±ÐºÐ°Ñ…)
    inString += inChar;
    if (inChar == ']') {
      if (inString.indexOf("[ON]")>0) {
        digitalWrite(PIN_LED, HIGH);
      }
      else if (inString.indexOf("[OFF]")>0) {
        digitalWrite(PIN_LED, LOW);
      }
      else
      {
        Serial.println("Wrong command");
      }
      inString = "";
    }
  }
}

