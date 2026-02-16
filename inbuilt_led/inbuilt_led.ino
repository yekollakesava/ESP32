#include <LiquidCrystal.h>
LiquidCrystal lcd(14, 27, 26, 25, 33, 32); // RS,E,D4,D5,D6,D7

void setup() {
  lcd.begin(16, 2);
  delay(200);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LCD Working!");
  lcd.setCursor(0,1);
  lcd.print("ESP32 Test");
}
void loop() {}

