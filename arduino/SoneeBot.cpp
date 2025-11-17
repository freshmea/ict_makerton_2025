#include "SoneeBot.hpp"

SoneeBot::SoneeBot(int s1Pin, int s2Pin, int neoPin, int neoCount,
                   int t1Pin, int t2Pin, int t3Pin, int buzPin)
{
    buzzerPin = buzPin;

    // 컴포넌트 객체 생성
    touch1 = new TouchSensor(t1Pin);
    touch2 = new TouchSensor(t2Pin);
    touch3 = new TouchSensor(t3Pin);
    servoController = new ServoController(s1Pin, s2Pin);
    displayManager = new DisplayManager(neoPin, neoCount);
    missionManager = new MissionManager();
}

SoneeBot::~SoneeBot()
{
    delete touch1;
    delete touch2;
    delete touch3;
    delete servoController;
    delete displayManager;
    delete missionManager;
}

void SoneeBot::init()
{
    Serial.begin(9600);

    // 각 컴포넌트 초기화
    touch1->init();
    touch2->init();
    touch3->init();
    servoController->init();
    displayManager->init();

    // 부저 핀 설정
    pinMode(buzzerPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // 초기화 메시지
    displayManager->lcdClear();
    displayManager->lcdPrint(0, 0, "SoneeBot Ready!");
    displayManager->lcdPrint(0, 1, "Initializing...");

    // 초기화 효과
    beepPattern(2, 100, 100);
    displayManager->rainbowEffect();

    delay(2000);

    Serial.println("=== SoneeBot Initialized ===");
    Serial.println("Touch Logic: LOW=NotTouched(1), HIGH=Touched(0)");
    printPinInfo();
}

void SoneeBot::update()
{
    unsigned long currentMillis = millis();

    // 서보 컨트롤러 업데이트 (비동기 움직임 처리)
    servoController->update(currentMillis);

    // 터치 센서 상태 업데이트
    touch1->update();
    touch2->update();
    touch3->update();

    // 터치 이벤트 처리
    if (touch1->isPressed())
    {
        missionManager->startGoodJobMessage();
        displayManager->showGoodJobMessage();
        Serial.println("Touch 1 PRESSED - Good Job message displayed");
    }

    // 터치별 부저 및 서보 처리
    if (touch1->getCurrentState() && touch1->getBeepCount() > 0)
    {
        static int lastTouch1Beep = 0;
        if (touch1->getBeepCount() > lastTouch1Beep)
        {
            servoController->executeMissionDecrease(currentMillis);
            beep(50);
            lastTouch1Beep = touch1->getBeepCount();
            Serial.println("Touch 1 (DECREASE) Beep #" + String(touch1->getBeepCount()));
        }
    }
    else if (!touch1->getCurrentState())
    {
        static int lastTouch1Released = 0;
        if (touch1->getLastBeepCount() != lastTouch1Released)
        {
            lastTouch1Released = touch1->getLastBeepCount();
            Serial.println("Touch 1 RELEASED - Total decrease: " + String(lastTouch1Released));
        }
    }

    if (touch2->getCurrentState() && touch2->getBeepCount() > 0)
    {
        static int lastTouch2Beep = 0;
        if (touch2->getBeepCount() > lastTouch2Beep)
        {
            beep(100);
            lastTouch2Beep = touch2->getBeepCount();
            Serial.println("Touch 2 (INCREASE) Beep #" + String(touch2->getBeepCount()));
        }
    }
    else if (!touch2->getCurrentState())
    {
        static int lastTouch2Released = 0;
        if (touch2->getLastBeepCount() != lastTouch2Released)
        {
            lastTouch2Released = touch2->getLastBeepCount();
            Serial.println("Touch 2 RELEASED - Total increase: " + String(lastTouch2Released));
        }
    }

    if (touch3->getCurrentState() && touch3->getBeepCount() > 0)
    {
        static int lastTouch3Beep = 0;
        if (touch3->getBeepCount() > lastTouch3Beep)
        {
            servoController->executeRandomMotion(currentMillis);
            beep(100);
            lastTouch3Beep = touch3->getBeepCount();
            Serial.println("Touch 3 Random Motion Beep #" + String(touch3->getBeepCount()));
        }
    }

    // 미션 상태 업데이트
    missionManager->update(touch1->getLastBeepCount(), touch2->getLastBeepCount(),
                           touch1->getBeepCount(), touch2->getBeepCount(),
                           touch1->getCurrentState(), touch2->getCurrentState());

    // 디스플레이 업데이트
    if (missionManager->isMissionCompleted())
    {
        if (missionManager->getMissionCompleteElapsed() == 0)
        {
            displayManager->showMissionCompleteMessage();
            // 완료 사운드
            beep(200);
            delay(100);
            beep(200);
            delay(100);
            beep(200);
            Serial.println("=== MISSION COMPLETED ===");
        }
        displayManager->missionCompleteEffect(missionManager->getMissionCompleteElapsed());

        // 서보 모터 동작 (미션 완료 효과)
        unsigned long elapsed = missionManager->getMissionCompleteElapsed();
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

                servoController->moveServo1(angle1);
                servoController->moveServo2(angle2);
            }
        }
    }
    else if (missionManager->isShowingGoodJob())
    {
        // Good Job 메시지 표시 중
    }
    else
    {
        // 일반 미션 디스플레이 업데이트
        int currentMissionCount = missionManager->calculateCurrentMissionCount(
            touch1->getLastBeepCount(), touch2->getLastBeepCount(),
            touch1->getBeepCount(), touch2->getBeepCount(),
            touch1->getCurrentState(), touch2->getCurrentState());

        displayManager->updateMissionDisplay(currentMissionCount,
                                             touch1->getCurrentState(),
                                             touch2->getCurrentState(),
                                             touch1->getBeepCount(),
                                             touch2->getBeepCount());
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
    Serial.println("Servo1: " + String(servoController->getServo1Angle()));
    Serial.println("Servo2: " + String(servoController->getServo2Angle()));
    Serial.println("Buzzer: " + String(buzzerPin));
    Serial.println("LCD: A4(SDA), A5(SCL)");
}

void SoneeBot::testAllDevices()
{
    Serial.println("Testing all devices...");
    displayManager->lcdPrint(0, 0, "Testing All");
    displayManager->lcdPrint(0, 1, "Devices...");

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
    displayManager->lcdPrint(0, 1, "Testing Servos");
    servoController->moveServo1(0);
    servoController->moveServo2(180);
    startMillis = millis();
    while (millis() - startMillis < 4000)
    {
        servoController->moveServoSmooth(1, 90, 1000);
        servoController->moveServoSmooth(2, 90, 1000);
        servoController->moveServoSmooth(1, 0, 1000);
        servoController->moveServoSmooth(2, 0, 1000);
        servoController->moveServoSmooth(1, 180, 1000);
        servoController->moveServoSmooth(2, 180, 1000);
        servoController->moveServoSmooth(1, 90, 1000);
        servoController->moveServoSmooth(2, 90, 1000);
        servoController->update(millis());
    }
}

void SoneeBot::testNeoPixels()
{
    Serial.println("Testing NeoPixels...");
    displayManager->lcdPrint(0, 1, "Testing LEDs");

    displayManager->fillColor(255, 0, 0);
    delay(300);
    displayManager->fillColor(0, 255, 0);
    delay(300);
    displayManager->fillColor(0, 0, 255);
    delay(300);
    displayManager->clearPixels();
}

void SoneeBot::testLCD()
{
    Serial.println("Testing LCD...");
    displayManager->lcdClear();
    displayManager->lcdPrint(0, 0, "LCD Test OK!");
    displayManager->lcdPrint(0, 1, "All Functions");
    delay(1000);
    displayManager->lcdBacklightOff();
    delay(500);
    displayManager->lcdBacklightOn();
}

void SoneeBot::testSensors()
{
    Serial.println("Testing sensors with duration tracking...");
    displayManager->lcdPrint(0, 1, "Touch & Hold");

    for (int i = 0; i < 100; i++)
    {
        update(); // 메인 업데이트 함수 사용
        delay(50);
    }
}
