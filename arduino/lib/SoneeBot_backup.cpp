#include "SoneeBot.hpp"

// 생성자
SoneeBot::SoneeBot(int s1Pin, int s2Pin, int neoPin, int neoCount,
                   int t1Pin, int t2Pin, int t3Pin, int buzPin)
{
    servo1Pin = s1Pin;
    servo2Pin = s2Pin;
    neoPixelPin = neoPin;
    neoPixelCount = neoCount;

    // 네오픽셀 개수를 4개로 설정
    if (neoPixelCount != 4)
    {
        neoPixelCount = 4;
    }

    touch1Pin = t1Pin;
    touch2Pin = t2Pin;
    touch3Pin = t3Pin;
    buzzerPin = buzPin;

    servo1Angle = 30;  // left - 30도로 변경 (0도 하늘 방향)
    servo2Angle = 150; // right - 150도로 변경 (180도 하늘 방향)
    lcdBacklight = true;

    // 터치 센서 상태 초기화
    touch1State = false;
    touch2State = false;
    touch3State = false;
    lastTouch1State = false;
    lastTouch2State = false;
    lastTouch3State = false;

    touch1StartTime = 0;
    touch2StartTime = 0;
    touch3StartTime = 0;
    touch1Duration = 0;
    touch2Duration = 0;
    touch3Duration = 0;

    // 터치별 부저 상태 초기화
    touch1BeepTriggered = false;
    touch2BeepTriggered = false;
    touch3BeepTriggered = false;

    // 터치별 부저 카운터 초기화
    touch1BeepCount = 0;
    touch2BeepCount = 0;
    touch3BeepCount = 0;

    // 터치 해제 시 마지막 카운트 초기화
    touch1LastBeepCount = 0;
    touch2LastBeepCount = 0;
    touch3LastBeepCount = 0;

    // 미션 카운터 초기화
    missionCount = 0;

    // 미션 완료 관련 변수 초기화
    missionCompleted = false;
    missionCompleteTime = 0;
    lastMissionCount = 0;

    // Good Job 메시지 관련 변수 초기화
    showingGoodJob = false;
    goodJobStartTime = 0;

    // LCD 상태 최적화 변수 초기화
    lastServo1Angle = -1;
    lastServo2Angle = -1;
    lastTouch1Display = false;
    lastTouch2Display = false;
    lastTouch3Display = false;
    lastTouch1DurationDisplay = 0;
    lastTouch2DurationDisplay = 0;
    lastTouch3DurationDisplay = 0;
    lastTouch3BeepCountDisplay = -1;
    lastTouch3StateDisplay = false;

    // 객체 생성
    strip = new Adafruit_NeoPixel(neoPixelCount, neoPixelPin, NEO_GRB + NEO_KHZ800);
    lcd = new LiquidCrystal_I2C(0x27, 16, 2);
}

// 소멸자
SoneeBot::~SoneeBot()
{
    delete strip;
    delete lcd;
}

// 초기화 함수
void SoneeBot::init()
{
    Serial.begin(9600);

    // 서보 모터 초기화
    servo1.attach(servo1Pin);
    servo2.attach(servo2Pin);
    servo1.write(servo1Angle);
    servo2.write(servo2Angle);

    // 네오픽셀 초기화
    strip->begin();
    strip->setBrightness(50);
    strip->show();

    // LCD 초기화
    lcd->begin();
    lcd->backlight();

    // 핀 모드 설정 - INPUT으로 변경 (풀업 저항 사용 안함)
    pinMode(touch1Pin, INPUT);
    pinMode(touch2Pin, INPUT);
    pinMode(touch3Pin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // 초기화 완료 메시지
    lcdClear();
    lcdPrint(0, 0, "SoneeBot Ready!");
    lcdPrint(0, 1, "Initializing...");

    // 초기화 효과
    beepPattern(2, 100, 100);
    rainbowEffect();

    delay(2000);
    updateStatusDisplay();

    Serial.println("=== SoneeBot Initialized ===");
    Serial.println("Touch Logic: LOW=NotTouched(1), HIGH=Touched(0)");
    printPinInfo();
}

// 서보 제어 함수들
void SoneeBot::moveServo1(int angle)
{
    angle = constrain(angle, 0, 180);
    servo1.write(angle);
    servo1Angle = angle;
}

void SoneeBot::moveServo2(int angle)
{
    angle = constrain(angle, 0, 180);
    servo2.write(angle);
    servo2Angle = angle;
}

void SoneeBot::moveBothServos(int angle1, int angle2)
{
    moveServo1(angle1);
    moveServo2(angle2);
}

void SoneeBot::moveServoSmooth(int servoNum, int angle, int delayMs)
{
    angle = constrain(angle, 0, 180);
    int currentAngle = (servoNum == 1) ? servo1Angle : servo2Angle;
    int step = (angle > currentAngle) ? 1 : -1;

    for (int pos = currentAngle; pos != angle; pos += step)
    {
        if (servoNum == 1)
        {
            servo1.write(pos);
            servo1Angle = pos;
        }
        else
        {
            servo2.write(pos);
            servo2Angle = pos;
        }
        delay(delayMs);
    }

    if (servoNum == 1)
    {
        servo1.write(angle);
        servo1Angle = angle;
    }
    else
    {
        servo2.write(angle);
        servo2Angle = angle;
    }
}

// 네오픽셀 제어 함수들
void SoneeBot::setPixelColor(int pixel, uint8_t r, uint8_t g, uint8_t b)
{
    if (pixel >= 0 && pixel < neoPixelCount)
    {
        strip->setPixelColor(pixel, strip->Color(r, g, b));
        strip->show();
    }
}

void SoneeBot::fillColor(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < neoPixelCount; i++)
    {
        strip->setPixelColor(i, strip->Color(r, g, b));
    }
    strip->show();
}

void SoneeBot::clearPixels()
{
    fillColor(0, 0, 0);
}

void SoneeBot::rainbowEffect()
{
    for (int j = 0; j < 256; j += 5)
    {
        for (int i = 0; i < neoPixelCount; i++)
        {
            strip->setPixelColor(i, strip->ColorHSV((i * 65536L / neoPixelCount) + j * 256));
        }
        strip->show();
        delay(10);
    }
}

// LCD 제어 함수들
void SoneeBot::lcdPrint(int col, int row, String text)
{
    lcd->setCursor(col, row);
    lcd->print(text);
}

void SoneeBot::lcdClear()
{
    lcd->clear();
}

void SoneeBot::lcdBacklightOn()
{
    lcd->backlight();
    lcdBacklight = true;
}

void SoneeBot::lcdBacklightOff()
{
    lcd->noBacklight();
    lcdBacklight = false;
}

void SoneeBot::updateStatusDisplay()
{
    // 변경사항 확인
    bool needUpdate = false;

    // 서보 각도 변경 확인
    if (servo1Angle != lastServo1Angle || servo2Angle != lastServo2Angle)
    {
        needUpdate = true;
    }

    // 터치 상태 변경 확인
    if (touch1State != lastTouch1Display ||
        touch2State != lastTouch2Display ||
        touch3State != lastTouch3Display)
    {
        needUpdate = true;
    }

    // 터치 시간 변경 확인 (1초 단위로만 체크)
    unsigned long currentTouch1Sec = touch1Duration / 1000;
    unsigned long currentTouch2Sec = touch2Duration / 1000;
    unsigned long currentTouch3Sec = touch3Duration / 1000;

    if (currentTouch1Sec != lastTouch1DurationDisplay ||
        currentTouch2Sec != lastTouch2DurationDisplay ||
        currentTouch3Sec != lastTouch3DurationDisplay)
    {
        needUpdate = true;
    }

    // 변경사항이 있을 때만 LCD 업데이트
    if (needUpdate)
    {
        lcdClear();
        lcdPrint(0, 0, "S1:" + String(servo1Angle) + " S2:" + String(servo2Angle));

        // 터치 상태와 지속 시간 표시 (0: 터치됨, 1: 터치안됨)
        String touchInfo = "T:";
        touchInfo += touch1State ? "0" : "1";
        touchInfo += touch2State ? "0" : "1";
        touchInfo += touch3State ? "0" : "1";

        // 터치되고 있는 센서들의 시간만 표시
        if (touch1State || touch2State || touch3State)
        {
            touchInfo += " ";
            if (touch1State)
                touchInfo += String(currentTouch1Sec) + "s ";
            if (touch2State)
                touchInfo += String(currentTouch2Sec) + "s ";
            if (touch3State)
                touchInfo += String(currentTouch3Sec) + "s";
        }

        lcdPrint(0, 1, touchInfo);

        // 이전 상태 업데이트
        lastServo1Angle = servo1Angle;
        lastServo2Angle = servo2Angle;
        lastTouch1Display = touch1State;
        lastTouch2Display = touch2State;
        lastTouch3Display = touch3State;
        lastTouch1DurationDisplay = currentTouch1Sec;
        lastTouch2DurationDisplay = currentTouch2Sec;
        lastTouch3DurationDisplay = currentTouch3Sec;
    }
}

// 미션 카운트에 따른 네오픽셀 업데이트 함수
void SoneeBot::updateMissionPixels(int missionCount)
{
    // 모든 픽셀을 먼저 끔
    clearPixels();

    if (missionCount <= 0)
    {
        // 미션이 0 이하면 모든 LED 꺼짐
        return;
    }

    // 미션 단계별 색상 설정
    for (int i = 0; i < min(missionCount, 12); i++)
    {                           // 최대 12개까지 (4×3단계)
        int pixelIndex = i % 4; // 0~3 순환

        if (i < 4)
        {
            // 1~4: 초록색
            strip->setPixelColor(pixelIndex, strip->Color(0, 255, 0));
        }
        else if (i < 8)
        {
            // 5~8: 파란색
            strip->setPixelColor(pixelIndex, strip->Color(0, 0, 255));
        }
        else
        {
            // 9~12: 빨간색
            strip->setPixelColor(pixelIndex, strip->Color(255, 0, 0));
        }
    }

    strip->show();
}

// 미션 완료 효과 함수
void SoneeBot::missionCompleteEffect()
{
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - missionCompleteTime;

    if (elapsed < 3000) // 3초 동안 효과 실행
    {
        // 무지개 네오픽셀 효과
        int hue = (elapsed / 10) % 360; // 10ms마다 색상 변경
        for (int i = 0; i < neoPixelCount; i++)
        {
            // 각 LED마다 다른 색상으로 무지개 효과
            int pixelHue = (hue + i * 90) % 360;                      // 90도씩 차이
            strip->setPixelColor(i, strip->ColorHSV(pixelHue * 182)); // 0-65535 범위로 변환
        }
        strip->show();

        // 서보 모터 동작 (3초 동안 2번 위아래 움직임)
        int cycle = elapsed / 750; // 750ms마다 한 사이클 (총 4사이클)
        int cycleTime = elapsed % 750;

        if (cycle < 4) // 총 4사이클 (2번 위아래)
        {
            int angle1, angle2;
            if (cycleTime < 375) // 전반부: 위로
            {
                float progress = (float)cycleTime / 375.0;
                angle1 = 30 + (int)(60 * sin(progress * PI / 2));  // 30도에서 90도로
                angle2 = 150 - (int)(60 * sin(progress * PI / 2)); // 150도에서 90도로
            }
            else // 후반부: 아래로
            {
                float progress = (float)(cycleTime - 375) / 375.0;
                angle1 = 90 - (int)(60 * sin(progress * PI / 2)); // 90도에서 30도로
                angle2 = 90 + (int)(60 * sin(progress * PI / 2)); // 90도에서 150도로
            }

            servo1.write(angle1);
            servo2.write(angle2);
        }
    }
    else
    {
        // 효과 완료 - 원래 상태로 복귀
        missionCompleted = false;
        servo1.write(30);
        servo2.write(150);
        servo1Angle = 30;
        servo2Angle = 150;
        clearPixels();
    }
}

// LCD 및 네오픽셀 메시지 업데이트 (최적화)
void SoneeBot::updateMessage()
{
    // 미션 완료 효과 실행 중이면 효과만 처리
    if (missionCompleted)
    {
        missionCompleteEffect();
        return;
    }

    // Good Job 메시지 표시 중인지 확인
    if (showingGoodJob)
    {
        unsigned long elapsed = millis() - goodJobStartTime;
        if (elapsed < 1000) // 1초 동안 표시
        {
            // Good Job 메시지 유지
            return;
        }
        else
        {
            // 1초 경과, Good Job 메시지 종료
            showingGoodJob = false;
            // 강제로 LCD 업데이트를 위해 이전 상태 초기화
            lastTouch3BeepCountDisplay = -999;
        }
    }

    // 최종 미션 카운터 계산 (2번으로 증가 - 1번으로 감소)
    int finalMissionCount = touch2LastBeepCount - touch1LastBeepCount;
    if (finalMissionCount < 0)
        finalMissionCount = 0; // 음수 방지

    // 현재 표시할 카운트 결정
    int displayCount;
    if (touch2State)
    {
        // 2번 터치 중: 기존 결과 + 현재 증가분
        int currentIncrease = touch2BeepCount;
        displayCount = finalMissionCount + currentIncrease;
    }
    else if (touch1State)
    {
        // 1번 터치 중: 기존 결과에서 현재 감소분 빼기
        int currentDecrease = touch1BeepCount;
        displayCount = finalMissionCount - currentDecrease;
        if (displayCount < 0)
            displayCount = 0;
    }
    else
    {
        // 터치하지 않은 상태: 최종 결과 표시
        displayCount = finalMissionCount;
    }

    // 디버깅을 위한 시리얼 출력
    Serial.print("Debug - lastMissionCount: ");
    Serial.print(lastMissionCount);
    Serial.print(", displayCount: ");
    Serial.print(displayCount);
    Serial.print(", touch1State: ");
    Serial.print(touch1State);
    Serial.print(", touch2State: ");
    Serial.println(touch2State);

    // 미션 완료 조건 체크: 이전 카운트가 1 이상이었는데 현재 0이 된 경우
    if (!missionCompleted && lastMissionCount >= 1 && displayCount == 0)
    {
        missionCompleted = true;
        missionCompleteTime = millis();

        lcdClear();
        lcdPrint(0, 0, "MISSION");
        lcdPrint(0, 1, "COMPLETED!");

        // 완료 사운드
        digitalWrite(buzzerPin, HIGH);
        delay(200);
        digitalWrite(buzzerPin, LOW);
        delay(100);
        digitalWrite(buzzerPin, HIGH);
        delay(200);
        digitalWrite(buzzerPin, LOW);
        delay(100);
        digitalWrite(buzzerPin, HIGH);
        delay(200);
        digitalWrite(buzzerPin, LOW);

        Serial.println("=== MISSION COMPLETED ===");
        Serial.println("Starting celebration effect for 3 seconds");
        Serial.print("Trigger condition - lastMissionCount: ");
        Serial.print(lastMissionCount);
        Serial.print(" -> displayCount: ");
        Serial.println(displayCount);

        // lastMissionCount를 즉시 업데이트하여 중복 실행 방지
        lastMissionCount = displayCount;
        return; // 미션 완료 효과로 넘어감
    }

    // 변경사항 확인
    bool needUpdate = false;

    if (displayCount != lastTouch3BeepCountDisplay ||
        touch2State != lastTouch3StateDisplay ||
        touch1State != lastTouch1Display)
    {
        needUpdate = true;
    }

    // 변경사항이 있을 때만 LCD 및 네오픽셀 업데이트
    if (needUpdate)
    {
        lcdClear();

        // 첫 번째 줄: "Today Mission!!" 표시
        lcdPrint(0, 0, "Today Mission!!");

        // 두 번째 줄: 미션 카운트 표시
        String missionInfo = ": ";
        missionInfo += String(displayCount);
        lcdPrint(0, 1, missionInfo);

        // 현재 터치 상태에 따른 메시지
        if (touch2State)
        {
            lcdPrint(4, 1, " (+ing...)");
        }
        else if (touch1State)
        {
            lcdPrint(4, 1, " (-ing...)");
        }
        else if (touch2LastBeepCount > 0 || touch1LastBeepCount > 0)
        {
            lcdPrint(4, 1, " (Done)");
        }

        // 네오픽셀 업데이트 (미션 카운트에 따라)
        updateMissionPixels(displayCount);

        // 이전 상태 업데이트
        lastTouch3BeepCountDisplay = displayCount;
        lastTouch3StateDisplay = touch2State;
        lastTouch1Display = touch1State;

        // 시리얼로 네오픽셀 상태 디버깅
        Serial.println("Mission Count: " + String(displayCount) + " - NeoPixel updated");
    }

    // lastMissionCount 업데이트는 마지막에 수행 (미션 완료가 실행되지 않은 경우에만)
    lastMissionCount = displayCount;
}

// 센서 읽기 함수들
// 기존 센서 읽기 함수들 (호환성 유지)
bool SoneeBot::readTouch1()
{
    return digitalRead(touch1Pin) == HIGH; // HIGH일 때 터치됨
}

bool SoneeBot::readTouch2()
{
    return digitalRead(touch2Pin) == HIGH; // HIGH일 때 터치됨
}

bool SoneeBot::readTouch3()
{
    return digitalRead(touch3Pin) == HIGH; // HIGH일 때 터치됨
}

// 새로운 터치 상태 업데이트 함수
void SoneeBot::updateTouchStates()
{
    unsigned long currentTime = millis();

    // 이전 상태 저장
    lastTouch1State = touch1State;
    lastTouch2State = touch2State;
    lastTouch3State = touch3State;

    // 현재 상태 읽기 (HIGH일 때 터치됨)
    touch1State = digitalRead(touch1Pin) == HIGH;
    touch2State = digitalRead(touch2Pin) == HIGH;
    touch3State = digitalRead(touch3Pin) == HIGH;

    // Touch 1 처리 (미션 감소용) - 역할 변경
    if (touch1State)
    {
        if (!lastTouch1State)
        {
            touch1StartTime = currentTime;
            touch1BeepCount = 0;

            // 1번 터치 시작 시 Good Job 메시지 표시
            showingGoodJob = true;
            goodJobStartTime = currentTime;
            lcdClear();
            lcdPrint(0, 0, "Good Job !!");
            Serial.println("Touch 1 PRESSED - Good Job message displayed");
        }
        touch1Duration = currentTime - touch1StartTime;

        int expectedBeepCount = (touch1Duration / 500) + 1;
        if (expectedBeepCount > touch1BeepCount)
        {
            // 미션 감소 시 서보 모션 추가
            moveBothServos(90, 90);  // 먼저 90도로 이동
            delay(300);              // 잠시 대기
            moveBothServos(30, 150); // 원래 위치로 복귀

            digitalWrite(buzzerPin, HIGH);
            delay(50); // 1번 터치는 짧은 소리 (감소용)
            digitalWrite(buzzerPin, LOW);
            touch1BeepCount++;
            Serial.println("Touch 1 (DECREASE) Beep #" + String(touch1BeepCount) + " at " + String(touch1Duration) + "ms - Servo motion executed");
        }
    }
    else
    {
        if (lastTouch1State)
        {
            // 1번 터치 해제 시: 기존 감소값에 현재 감소값 추가
            touch1LastBeepCount += touch1BeepCount;
            Serial.println("Touch 1 RELEASED - Current decrease: " + String(touch1BeepCount) + ", Total decrease: " + String(touch1LastBeepCount));
        }
        touch1Duration = 0;
        touch1StartTime = 0;
        touch1BeepCount = 0;
    }

    // Touch 2 처리 (미션 증가용) - 역할 변경
    if (touch2State)
    {
        if (!lastTouch2State)
        {
            touch2StartTime = currentTime;
            touch2BeepCount = 0;
        }
        touch2Duration = currentTime - touch2StartTime;

        int expectedBeepCount = (touch2Duration / 500) + 1;
        if (expectedBeepCount > touch2BeepCount)
        {
            digitalWrite(buzzerPin, HIGH);
            delay(100); // 2번 터치는 긴 소리 (증가용)
            digitalWrite(buzzerPin, LOW);
            touch2BeepCount++;
            Serial.println("Touch 2 (INCREASE) Beep #" + String(touch2BeepCount) + " at " + String(touch2Duration) + "ms");
        }
    }
    else
    {
        if (lastTouch2State)
        {
            // 2번 터치 해제 시: 기존 증가값에 현재 증가값 추가
            touch2LastBeepCount += touch2BeepCount;
            Serial.println("Touch 2 RELEASED - Current increase: " + String(touch2BeepCount) + ", Total increase: " + String(touch2LastBeepCount));
        }
        touch2Duration = 0;
        touch2StartTime = 0;
        touch2BeepCount = 0;
    }

    // Touch 3 처리 - 랜덤 서보 선택 기능으로 변경
    if (touch3State)
    {
        if (!lastTouch3State)
        {
            touch3StartTime = currentTime;
            touch3BeepCount = 0;
        }
        touch3Duration = currentTime - touch3StartTime;

        int expectedBeepCount = (touch3Duration / 500) + 1;
        if (expectedBeepCount > touch3BeepCount)
        {
            // 랜덤으로 서보1 또는 서보2 선택 (1 또는 2)
            int selectedServo = random(1, 3); // 1 또는 2 반환
            int originalAngle1 = servo1Angle;
            int originalAngle2 = servo2Angle;

            if (selectedServo == 1)
            {
                // 서보1 선택: 90도로 내려갔다가 원래 위치로 복귀
                Serial.println("Touch 3 - Servo1 selected for random motion");
                moveServo1(90);
                delay(500);                 // 90도에서 0.5초 대기
                moveServo1(originalAngle1); // 원래 위치로 복귀
                Serial.println("Touch 3 - Servo1 motion completed");
            }
            else
            {
                // 서보2 선택: 90도로 내려갔다가 원래 위치로 복귀
                Serial.println("Touch 3 - Servo2 selected for random motion");
                moveServo2(90);
                delay(500);                 // 90도에서 0.5초 대기
                moveServo2(originalAngle2); // 원래 위치로 복귀
                Serial.println("Touch 3 - Servo2 motion completed");
            }

            digitalWrite(buzzerPin, HIGH);
            delay(100);
            digitalWrite(buzzerPin, LOW);
            touch3BeepCount++;
            Serial.println("Touch 3 Beep #" + String(touch3BeepCount) + " at " + String(touch3Duration) + "ms - Random servo" + String(selectedServo) + " motion executed");
        }
    }
    else
    {
        if (lastTouch3State)
        {
            touch3LastBeepCount = touch3BeepCount;
            Serial.println("Touch 3 RELEASED - Total beeps: " + String(touch3LastBeepCount));
        }
        touch3Duration = 0;
        touch3StartTime = 0;
        touch3BeepCount = 0;
    }
}

// 터치가 방금 눌렸는지 확인
bool SoneeBot::isTouchPressed(int touchNum)
{
    switch (touchNum)
    {
    case 1:
        return touch1State && !lastTouch1State;
    case 2:
        return touch2State && !lastTouch2State;
    case 3:
        return touch3State && !lastTouch3State;
    default:
        return false;
    }
}

// 터치가 방금 떼어졌는지 확인
bool SoneeBot::isTouchReleased(int touchNum)
{
    switch (touchNum)
    {
    case 1:
        return !touch1State && lastTouch1State;
    case 2:
        return !touch2State && lastTouch2State;
    case 3:
        return !touch3State && lastTouch3State;
    default:
        return false;
    }
}

// 터치가 현재 눌려있는지 확인
bool SoneeBot::isTouchHeld(int touchNum)
{
    switch (touchNum)
    {
    case 1:
        return touch1State;
    case 2:
        return touch2State;
    case 3:
        return touch3State;
    default:
        return false;
    }
}

// 터치 지속 시간 반환 (밀리초)
unsigned long SoneeBot::getTouchDuration(int touchNum)
{
    switch (touchNum)
    {
    case 1:
        return touch1Duration;
    case 2:
        return touch2Duration;
    case 3:
        return touch3Duration;
    default:
        return 0;
    }
}

// 롱프레스 여부 확인
bool SoneeBot::isTouchLongPress(int touchNum, unsigned long threshold)
{
    return isTouchHeld(touchNum) && getTouchDuration(touchNum) >= threshold;
}

// 부저 제어 함수들
void SoneeBot::beep(int duration)
{
    digitalWrite(buzzerPin, HIGH);
    delay(duration);
    digitalWrite(buzzerPin, LOW);
}

void SoneeBot::beepPattern(int count, int duration, int interval)
{
    for (int i = 0; i < count; i++)
    {
        beep(duration);
        if (i < count - 1)
            delay(interval);
    }
}

// 상태 정보 함수들
int SoneeBot::getServo1Angle()
{
    return servo1Angle;
}

int SoneeBot::getServo2Angle()
{
    return servo2Angle;
}

void SoneeBot::printPinInfo()
{
    Serial.println("Pin Configuration:");
    Serial.println("Servo1: " + String(servo1Pin));
    Serial.println("Servo2: " + String(servo2Pin));
    Serial.println("NeoPixel: " + String(neoPixelPin) + " (Count: " + String(neoPixelCount) + ")");
    Serial.println("Touch1: " + String(touch1Pin));
    Serial.println("Touch2: " + String(touch2Pin));
    Serial.println("Touch3: " + String(touch3Pin));
    Serial.println("Buzzer: " + String(buzzerPin));
    Serial.println("LCD: A4(SDA), A5(SCL)");
}

// 테스트 함수들
void SoneeBot::testAllDevices()
{
    Serial.println("Testing all devices...");

    lcdPrint(0, 0, "Testing All");
    lcdPrint(0, 1, "Devices...");

    testServos();
    delay(1000);
    testNeoPixels();
    delay(1000);
    testSensors();
    delay(1000);
    beepPattern(3, 100, 100);

    updateStatusDisplay();
    Serial.println("All tests completed!");
}

void SoneeBot::testServos()
{
    Serial.println("Testing servos...");
    lcdPrint(0, 1, "Testing Servos");

    moveBothServos(0, 180);
    delay(500);
    moveBothServos(180, 0);
    delay(500);
    moveBothServos(90, 90);
}

void SoneeBot::testNeoPixels()
{
    Serial.println("Testing NeoPixels...");
    lcdPrint(0, 1, "Testing LEDs");

    fillColor(255, 0, 0);
    delay(300);
    fillColor(0, 255, 0);
    delay(300);
    fillColor(0, 0, 255);
    delay(300);
    clearPixels();
}

void SoneeBot::testLCD()
{
    Serial.println("Testing LCD...");
    lcdClear();
    lcdPrint(0, 0, "LCD Test OK!");
    lcdPrint(0, 1, "All Functions");
    delay(1000);
    lcdBacklightOff();
    delay(500);
    lcdBacklightOn();
}

void SoneeBot::testSensors()
{
    Serial.println("Testing sensors with duration tracking...");
    lcdPrint(0, 1, "Touch & Hold");

    for (int i = 0; i < 100; i++)
    {
        updateTouchStates();

        // 터치 시작 감지
        if (isTouchPressed(1))
        {
            Serial.println("Touch 1 PRESSED");
            beep(50);
        }
        if (isTouchPressed(2))
        {
            Serial.println("Touch 2 PRESSED");
            beep(50);
        }
        if (isTouchPressed(3))
        {
            Serial.println("Touch 3 PRESSED");
            beep(50);
        }

        // 롱프레스 감지 (1초)
        if (isTouchLongPress(1, 1000))
        {
            Serial.println("Touch 1 LONG PRESS: " + String(getTouchDuration(1)) + "ms");
            fillColor(255, 0, 0);
        }
        if (isTouchLongPress(2, 1000))
        {
            Serial.println("Touch 2 LONG PRESS: " + String(getTouchDuration(2)) + "ms");
            fillColor(0, 255, 0);
        }
        if (isTouchLongPress(3, 1000))
        {
            Serial.println("Touch 3 LONG PRESS: " + String(getTouchDuration(3)) + "ms");
            fillColor(0, 0, 255);
        }

        // 터치 해제 감지 - 해제될 때만 최종 시간 출력
        if (isTouchReleased(1))
        {
            Serial.println("Touch 1 RELEASED after " + String(getTouchDuration(1)) + "ms");
            clearPixels();
        }
        if (isTouchReleased(2))
        {
            Serial.println("Touch 2 RELEASED after " + String(getTouchDuration(2)) + "ms");
            clearPixels();
        }
        if (isTouchReleased(3))
        {
            Serial.println("Touch 3 RELEASED after " + String(getTouchDuration(3)) + "ms");
            clearPixels();
        }

        // 현재 터치 상태 디버깅 출력 (5초마다)
        if (i % 100 == 0)
        {
            Serial.print("Touch states: ");
            Serial.print(touch1State ? "1" : "0");
            Serial.print(touch2State ? "1" : "0");
            Serial.print(touch3State ? "1" : "0");
            Serial.print(" Durations: ");
            Serial.print(touch1Duration);
            Serial.print(", ");
            Serial.print(touch2Duration);
            Serial.print(", ");
            Serial.println(touch3Duration);
        }

        delay(50);
    }
}
