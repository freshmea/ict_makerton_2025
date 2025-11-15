#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>

// ===== 핀 정의 =====
#define TOUCH_PIN1 8
#define TOUCH_PIN2 7
#define TOUCH_PIN3 4
#define BUZZER_PIN 2
#define NEOPIXEL_PIN 3
#define SERVO1_PIN 10
#define SERVO2_PIN 11
// LCD는 A4(SDA), A5(SCL) 사용

// ===== 객체 생성 =====
Servo servo1;
Servo servo2;
Adafruit_NeoPixel strip(8, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== 테스트 상태 변수 =====
int testMode = 0; // 0~6: 각 장치 테스트 모드
unsigned long lastTestTime = 0;
const unsigned long TEST_INTERVAL = 3000; // 3초마다 다음 테스트

// ===== 터치 센서 상태 =====
bool lastTouch1 = false, lastTouch2 = false, lastTouch3 = false;

void setup()
{
    Serial.begin(9600);

    // 핀 초기화
    pinMode(TOUCH_PIN1, INPUT_PULLUP);
    pinMode(TOUCH_PIN2, INPUT_PULLUP);
    pinMode(TOUCH_PIN3, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // 객체 초기화
    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    strip.begin();
    strip.setBrightness(50);
    strip.show();
    lcd.begin();
    lcd.backlight();

    // 시작 메시지
    Serial.println("=== Arduino Device Test System ===");
    Serial.println("Testing: Touch(8,7,4), Buzzer(2), NeoPixel(3), Servo(10,11), LCD(A4,A5)");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Device Test");
    lcd.setCursor(0, 1);
    lcd.print("Starting...");
    delay(2000);
}

void loop()
{
    unsigned long currentTime = millis();

    // 3초마다 테스트 모드 변경
    if (currentTime - lastTestTime >= TEST_INTERVAL)
    {
        lastTestTime = currentTime;
        testMode = (testMode + 1) % 7;

        Serial.print("Test Mode: ");
        Serial.println(testMode);
    }

    // 현재 테스트 모드에 따른 장치 테스트
    switch (testMode)
    {
    case 0:
        testTouchSensors();
        break;
    case 1:
        testBuzzer();
        break;
    case 2:
        testNeoPixel();
        break;
    case 3:
        testServo1();
        break;
    case 4:
        testServo2();
        break;
    case 5:
        testLCD();
        break;
    case 6:
        testAll();
        break;
    }

    delay(100);
}

void testTouchSensors()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Touch Test");

    bool touch1 = !digitalRead(TOUCH_PIN1);
    bool touch2 = !digitalRead(TOUCH_PIN2);
    bool touch3 = !digitalRead(TOUCH_PIN3);

    // 터치 상태 변화 감지
    if (touch1 != lastTouch1)
    {
        Serial.print("Touch 1 (Pin 8): ");
        Serial.println(touch1 ? "PRESSED" : "RELEASED");
        lastTouch1 = touch1;
    }
    if (touch2 != lastTouch2)
    {
        Serial.print("Touch 2 (Pin 7): ");
        Serial.println(touch2 ? "PRESSED" : "RELEASED");
        lastTouch2 = touch2;
    }
    if (touch3 != lastTouch3)
    {
        Serial.print("Touch 3 (Pin 4): ");
        Serial.println(touch3 ? "PRESSED" : "RELEASED");
        lastTouch3 = touch3;
    }

    // LCD에 터치 상태 표시
    lcd.setCursor(0, 1);
    lcd.print("8:");
    lcd.print(touch1 ? "ON " : "OFF");
    lcd.print("7:");
    lcd.print(touch2 ? "ON " : "OFF");
    lcd.print("4:");
    lcd.print(touch3 ? "ON" : "OFF");

    // LED로 터치 상태 표시
    digitalWrite(LED_BUILTIN, touch1 || touch2 || touch3);
}

void testBuzzer()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Buzzer Test");
    lcd.setCursor(0, 1);
    lcd.print("Pin 2 - Beeping");

    Serial.println("Testing Buzzer (Pin 2)");

    // 비프음 패턴
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
    }
}

void testNeoPixel()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NeoPixel Test");
    lcd.setCursor(0, 1);
    lcd.print("Pin 3 - Colors");

    Serial.println("Testing NeoPixel (Pin 3)");

    static int colorIndex = 0;
    uint32_t colors[] = {
        strip.Color(255, 0, 0),    // 빨강
        strip.Color(0, 255, 0),    // 초록
        strip.Color(0, 0, 255),    // 파랑
        strip.Color(255, 255, 0),  // 노랑
        strip.Color(255, 0, 255),  // 마젠타
        strip.Color(0, 255, 255),  // 시안
        strip.Color(255, 255, 255) // 흰색
    };

    for (int i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, colors[colorIndex]);
    }
    strip.show();

    colorIndex = (colorIndex + 1) % 7;
}

void testServo1()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Servo 1 Test");
    lcd.setCursor(0, 1);
    lcd.print("Pin 10 - Moving");

    Serial.println("Testing Servo 1 (Pin 10)");

    static int angle1 = 0;
    static bool direction1 = true;

    servo1.write(angle1);

    if (direction1)
    {
        angle1 += 5;
        if (angle1 >= 180)
            direction1 = false;
    }
    else
    {
        angle1 -= 5;
        if (angle1 <= 0)
            direction1 = true;
    }
}

void testServo2()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Servo 2 Test");
    lcd.setCursor(0, 1);
    lcd.print("Pin 11 - Moving");

    Serial.println("Testing Servo 2 (Pin 11)");

    static int angle2 = 180;
    static bool direction2 = false;

    servo2.write(angle2);

    if (direction2)
    {
        angle2 += 5;
        if (angle2 >= 180)
            direction2 = false;
    }
    else
    {
        angle2 -= 5;
        if (angle2 <= 0)
            direction2 = true;
    }
}

void testLCD()
{
    Serial.println("Testing LCD (A4-SDA, A5-SCL)");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LCD Test OK!");
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(millis() / 1000);
    lcd.print("s");

    // LCD 백라이트 깜빡임
    static bool backlightState = true;
    if (backlightState)
    {
        lcd.backlight();
    }
    else
    {
        lcd.noBacklight();
    }
    backlightState = !backlightState;
}

void testAll()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All Device Test");
    lcd.setCursor(0, 1);
    lcd.print("Check Serial!");

    Serial.println("=== ALL DEVICE STATUS ===");

    // 터치 센서 상태
    Serial.print("Touch Sensors - 8:");
    Serial.print(!digitalRead(TOUCH_PIN1) ? "ON" : "OFF");
    Serial.print(" 7:");
    Serial.print(!digitalRead(TOUCH_PIN2) ? "ON" : "OFF");
    Serial.print(" 4:");
    Serial.println(!digitalRead(TOUCH_PIN3) ? "ON" : "OFF");

    // 부저 테스트
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Buzzer: BEEPED");

    // 서보 중간 위치
    servo1.write(90);
    servo2.write(90);
    Serial.println("Servos: CENTERED");

    // 네오픽셀 흰색
    for (int i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, strip.Color(100, 100, 100));
    }
    strip.show();
    Serial.println("NeoPixel: WHITE");

    Serial.println("========================");
}
