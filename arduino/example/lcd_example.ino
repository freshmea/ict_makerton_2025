#include <LiquidCrystal.h>

// LCD 핀 연결 설정 (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
    // LCD 초기화 (16x2 크기)
    lcd.begin(16, 2);

    // 첫 번째 줄에 메시지 출력
    lcd.print("Hello, World!");

    // 두 번째 줄로 커서 이동 후 메시지 출력
    lcd.setCursor(0, 1);
    lcd.print("LCD Test!");
}

void loop()
{
    // 메인 루프에서는 아무것도 하지 않음
}
