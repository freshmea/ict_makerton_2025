#include "SoneeBot.hpp"

SoneeBot::SoneeBot(int s1Pin, int s2Pin, int neoPin, int neoCount,
                   int t1Pin, int t2Pin, int t3Pin, int buzPin)
    : touch1(t1Pin), touch2(t2Pin), touch3(t3Pin), servoController(s1Pin, s2Pin), displayManager(neoPin, neoCount)
{
    Serial.begin(115200);
    Serial.println("SoneeBot creating (all static allocation)...");
    Serial.print("Free RAM after creation: ");
    Serial.println(getFreeMemory());

    buzzerPin = buzPin;
    Serial.println("SoneeBot object creation completed");
}

SoneeBot::~SoneeBot()
{
    // 모든 객체가 정적 할당이므로 delete 불필요
}

void SoneeBot::init()
{
    Serial.begin(9600);

    // 각 컴포넌트 초기화
    touch1.init();
    touch2.init();
    touch3.init();
    servoController.init();
    displayManager.init();

    // 부저 핀 설정
    pinMode(buzzerPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // 초기화 메시지
    displayManager.lcdClear();
    displayManager.lcdPrint(0, 0, "SoneeBot Ready!");
    displayManager.lcdPrint(0, 1, "Initializing...");

    // 초기화 효과
    beepPattern(2, 100, 100);
    displayManager.rainbowEffect();

    delay(2000);

    Serial.println("=== SoneeBot Initialized ===");
    Serial.println("Touch Logic: LOW=NotTouched(1), HIGH=Touched(0)");
    printPinInfo();
}

void SoneeBot::update()
{
    unsigned long currentMillis = millis();

    // 서보 컨트롤러 업데이트 (비동기 움직임 처리)
    servoController.update(currentMillis);

    // 터치 센서 상태 업데이트
    touch1.update();
    touch2.update();
    touch3.update();

    // 터치 이벤트 처리
    if (touch1.isPressed())
    {
        missionManager.startGoodJobMessage();
        displayManager.showGoodJobMessage();
        Serial.println("Touch 1 PRESSED - Good Job message displayed");
    }

    // 터치별 부저 및 서보 처리
    if (touch1.getCurrentState() && touch1.getBeepCount() > 0)
    {
        static int lastTouch1Beep = 0;
        if (touch1.getBeepCount() > lastTouch1Beep)
        {
            servoController.executeMissionDecrease(currentMillis);
            beep(50);
            lastTouch1Beep = touch1.getBeepCount();
            Serial.println("Touch 1 (DECREASE) Beep #" + String(touch1.getBeepCount()));
        }
    }
    else if (!touch1.getCurrentState())
    {
        static int lastTouch1Released = 0;
        if (touch1.getLastBeepCount() != lastTouch1Released)
        {
            lastTouch1Released = touch1.getLastBeepCount();
            Serial.println("Touch 1 RELEASED - Total decrease: " + String(lastTouch1Released));
        }
    }

    if (touch2.getCurrentState() && touch2.getBeepCount() > 0)
    {
        static int lastTouch2Beep = 0;
        if (touch2.getBeepCount() > lastTouch2Beep)
        {
            beep(100);
            lastTouch2Beep = touch2.getBeepCount();
            Serial.println("Touch 2 (INCREASE) Beep #" + String(touch2.getBeepCount()));
        }
    }
    else if (!touch2.getCurrentState())
    {
        static int lastTouch2Released = 0;
        if (touch2.getLastBeepCount() != lastTouch2Released)
        {
            lastTouch2Released = touch2.getLastBeepCount();
            Serial.println("Touch 2 RELEASED - Total increase: " + String(lastTouch2Released));
        }
    }

    if (touch3.getCurrentState() && touch3.getBeepCount() > 0)
    {
        static int lastTouch3Beep = 0;
        if (touch3.getBeepCount() > lastTouch3Beep)
        {
            servoController.executeRandomMotion(currentMillis);
            beep(100);
            lastTouch3Beep = touch3.getBeepCount();
            Serial.println("Touch 3 Random Motion Beep #" + String(touch3.getBeepCount()));
        }
    }

    // 미션 상태 업데이트
    missionManager.update(touch1.getLastBeepCount(), touch2.getLastBeepCount(),
                          touch1.getBeepCount(), touch2.getBeepCount(),
                          touch1.getCurrentState(), touch2.getCurrentState());

    // 디스플레이 업데이트
    if (missionManager.isMissionCompleted())
    {
        if (missionManager.getMissionCompleteElapsed() == 0)
        {
            displayManager.showMissionCompleteMessage();
            // 완료 사운드
            beep(200);
            delay(100);
            beep(200);
            delay(100);
            beep(200);
            Serial.println("=== MISSION COMPLETED ===");
        }
        displayManager.missionCompleteEffect(missionManager.getMissionCompleteElapsed());

        // 서보 모터 동작 (미션 완료 효과)
        unsigned long elapsed = missionManager.getMissionCompleteElapsed();
        if (elapsed < 3000)
        {
            int cycle = elapsed / 750;
            int cycleTime = elapsed % 750;

            if (cycle < 4)
            {
                int angle1, angle2;
                if (cycleTime < 375)
                {
                    float progress = (float)cycleTime / 375.0;
                    angle1 = 30 + (int)(60 * sin(progress * PI / 2));
                    angle2 = 150 - (int)(60 * sin(progress * PI / 2));
                }
                else
                {
                    float progress = (float)(cycleTime - 375) / 375.0;
                    angle1 = 90 - (int)(60 * sin(progress * PI / 2));
                    angle2 = 90 + (int)(60 * sin(progress * PI / 2));
                }

                servoController.servo1Async.moveToAngle(angle1);
                servoController.servo2Async.moveToAngle(angle2);
            }
        }
    }
    else if (missionManager.isShowingGoodJob())
    {
        // Good Job 메시지 표시 중
    }
    else
    {
        // 일반 미션 디스플레이 업데이트
        int currentMissionCount = missionManager.calculateCurrentMissionCount(
            touch1.getLastBeepCount(), touch2.getLastBeepCount(),
            touch1.getBeepCount(), touch2.getBeepCount(),
            touch1.getCurrentState(), touch2.getCurrentState());

        displayManager.updateMissionDisplay(currentMissionCount,
                                            touch1.getCurrentState(),
                                            touch2.getCurrentState(),
                                            touch1.getBeepCount(),
                                            touch2.getBeepCount());
    }
}

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

void SoneeBot::printPinInfo()
{
    Serial.println("Pin Configuration:");
    Serial.println("Servo1: " + String(servoController.servo1Async.getCurrentAngle()));
    Serial.println("Servo2: " + String(servoController.servo2Async.getCurrentAngle()));
    Serial.println("Buzzer: " + String(buzzerPin));
    Serial.println("LCD: A4(SDA), A5(SCL)");
}

void SoneeBot::testAllDevices()
{
    Serial.println("Testing all devices...");
    displayManager.lcdPrint(0, 0, "Testing All");
    displayManager.lcdPrint(0, 1, "Devices...");

    testServos();
    delay(1000);
    testNeoPixels();
    delay(1000);
    testSensors();
    delay(1000);
    beepPattern(3, 100, 100);

    Serial.println("All tests completed!");
}

void SoneeBot::testServos()
{
    Serial.println("Testing servos...");
    displayManager.lcdPrint(0, 1, "Testing Servos");

    unsigned long currentMillis = millis();

    servoController.servo1Async.moveToAngle(0);
    servoController.servo2Async.moveToAngle(180);
    delay(500);

    servoController.servo1Async.moveSmooth(90, 1000, currentMillis);
    servoController.servo2Async.moveSmooth(90, 1000, currentMillis);
    delay(1000);

    servoController.servo1Async.moveSmooth(180, 1000, currentMillis + 1000);
    servoController.servo2Async.moveSmooth(0, 1000, currentMillis + 1000);
    delay(1000);

    servoController.servo1Async.moveToAngle(30);
    servoController.servo2Async.moveToAngle(150);
}

void SoneeBot::testNeoPixels()
{
    Serial.println("Testing NeoPixels...");
    displayManager.lcdPrint(0, 1, "Testing LEDs");

    displayManager.fillColor(255, 0, 0);
    delay(300);
    displayManager.fillColor(0, 255, 0);
    delay(300);
    displayManager.fillColor(0, 0, 255);
    delay(300);
    displayManager.clearPixels();
}

void SoneeBot::testLCD()
{
    Serial.println("Testing LCD...");
    displayManager.lcdClear();
    displayManager.lcdPrint(0, 0, "LCD Test OK!");
    displayManager.lcdPrint(0, 1, "All Functions");
    delay(1000);
    displayManager.lcdBacklightOff();
    delay(500);
    displayManager.lcdBacklightOn();
}

void SoneeBot::testSensors()
{
    Serial.println("Testing sensors with duration tracking...");
    displayManager.lcdPrint(0, 1, "Touch & Hold");

    for (int i = 0; i < 100; i++)
    {
        update(); // 메인 업데이트 함수 사용
        delay(50);
    }
}

// 메모리 체크 함수 추가
int SoneeBot::getFreeMemory()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
