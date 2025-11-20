#include <LiquidCrystal_I2C.h>

void setup()
{
    LiquidCrystal_I2C lcd(0x27, 16, 2);
    lcd.init(); // initialize the lcd
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Setup Complete");
}

void loop()
{
}
