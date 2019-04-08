// buzzer test
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
const int beep = 12;
void setup()
{
  lcd.init();
  lcd.backlight();
  pinMode(beep, OUTPUT);
}
void loop()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ALERT!");
  lcd.setCursor(0,1);
  lcd.print("Reservoir Full");
  tone(beep, 1000);
  delay(500);
  lcd.clear();
  noTone(beep);
  delay(500);
}
