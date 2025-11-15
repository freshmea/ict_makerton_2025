#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>

// ===== 서보 모터 설정 =====
Servo servo10;
Servo servo11;

// ===== LCD 설정 =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== 서보 상태 변수 =====
int servo10_angle = 90; // 서보10 현재 각도
int servo11_angle = 90; // 서보11 현재 각도

// ===== 시리얼 입력 처리 변수 =====
String inputString = "";
boolean stringComplete = false;

void setup()
{
    // 시리얼 통신 시작
    Serial.begin(9600);

    // 서보 모터 핀 연결 및 초기화
    servo10.attach(10);
    servo11.attach(11);
    servo10.write(servo10_angle);
    servo11.write(servo11_angle);

    // LCD 초기화
    lcd.begin();
    lcd.backlight();

    // 초기 화면 표시
    updateLCD();

    // 사용법 안내
    Serial.println("Servo Controller Ready!");
    Serial.println("Usage: [pin] [angle]");
    Serial.println("Example: 10 180 (move servo on pin 10 to 180 degrees)");
    Serial.println("Pin: 10 or 11, Angle: 0-180");
}

void loop()
{
    // 시리얼 입력을 실시간으로 처리
    while (Serial.available())
    {
        char inChar = (char)Serial.read();

        if (inChar == '\n' || inChar == '\r')
        {
            if (inputString.length() > 0)
            {
                stringComplete = true;
            }
        }
        else
        {
            inputString += inChar;
            // 입력 중인 문자를 LCD에 실시간 표시
            displayInputOnLCD();
        }
    }

    // 시리얼 입력 처리
    if (stringComplete)
    {
        processCommand(inputString);
        inputString = "";
        stringComplete = false;
    }
}

// 입력 중인 명령을 LCD에 표시하는 함수
void displayInputOnLCD()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Input:");
    lcd.setCursor(0, 1);
    lcd.print(inputString);
}

// 명령 처리 함수
void processCommand(String command)
{
    command.trim(); // 공백 제거

    // 디버깅: 받은 명령어를 시리얼과 LCD에 출력
    Serial.print("Received command: '");
    Serial.print(command);
    Serial.println("'");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cmd:");
    lcd.print(command);

    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1)
    {
        Serial.println("Error: Invalid format. Use: [pin] [angle]");
        lcd.setCursor(0, 1);
        lcd.print("Invalid format");
        delay(2000);
        updateLCD();
        return;
    }

    String pinStr = command.substring(0, spaceIndex);
    String angleStr = command.substring(spaceIndex + 1);

    int pin = pinStr.toInt();
    int angle = angleStr.toInt();

    // 디버깅: 파싱된 값들을 시리얼과 LCD에 출력 (88-89번 라인 해당)
    Serial.print("Parsed - Pin: ");
    Serial.print(pin);
    Serial.print(", Angle: ");
    Serial.println(angle);

    lcd.setCursor(0, 1);
    lcd.print("P:");
    lcd.print(pin);
    lcd.print(" A:");
    lcd.print(angle);
    delay(1000); // 디버깅 정보 표시 시간

    // 입력값 검증
    if ((pin != 10 && pin != 11) || angle < 0 || angle > 180)
    {
        Serial.println("Error: Pin must be 10 or 11, Angle must be 0-180");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error:");
        lcd.setCursor(0, 1);
        lcd.print("Pin10/11,0-180");
        delay(2000);
        updateLCD();
        return;
    }

    // 서보 모터 제어
    moveServo(pin, angle);

    // LCD 업데이트
    updateLCD();

    // 시리얼 확인 메시지
    Serial.print("Moved servo on pin ");
    Serial.print(pin);
    Serial.print(" to ");
    Serial.print(angle);
    Serial.println(" degrees");
}

// 서보 모터 이동 함수
void moveServo(int pin, int targetAngle)
{
    int currentAngle = (pin == 10) ? servo10_angle : servo11_angle;

    // 부드러운 움직임을 위한 단계별 이동
    int step = (targetAngle > currentAngle) ? 1 : -1;

    for (int angle = currentAngle; angle != targetAngle; angle += step)
    {
        if (pin == 10)
        {
            servo10.write(angle);
            servo10_angle = angle;
        }
        else
        {
            servo11.write(angle);
            servo11_angle = angle;
        }
        delay(15); // 부드러운 움직임을 위한 딜레이
    }

    // 최종 위치 설정
    if (pin == 10)
    {
        servo10.write(targetAngle);
        servo10_angle = targetAngle;
    }
    else
    {
        servo11.write(targetAngle);
        servo11_angle = targetAngle;
    }
}

// LCD 업데이트 함수
void updateLCD()
{
    lcd.clear();

    // 첫 번째 줄: Servo 10 상태
    lcd.setCursor(0, 0);
    lcd.print("S10:");
    lcd.print(servo10_angle);
    lcd.print("deg");

    // 두 번째 줄: Servo 11 상태
    lcd.setCursor(0, 1);
    lcd.print("S11:");
    lcd.print(servo11_angle);
    lcd.print("deg");

    // 우측에 상태 표시
    lcd.setCursor(10, 0);
    lcd.print("READY");

    // 하단에 입력 대기 표시
    lcd.setCursor(10, 1);
    lcd.print("WAIT");
}
