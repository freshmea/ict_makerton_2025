#include "SoneeBot.hpp"

SoneeBot::SoneeBot(int s1Pin, int s2Pin, int neoPin, int neoCount,
                   int t1Pin, int t2Pin, int t3Pin, int buzPin)
{
    // 모듈 객체 생성
    touch1 = new TouchSensor(t1Pin);
    touch2 = new TouchSensor(t2Pin);
    touch3 = new TouchSensor(t3Pin);
    servoController = new ServoController(s1Pin, s2Pin);
    servoAsync = new ServoAsync(servoController);
    displayManager = new DisplayManager(neoPin, neoCount);
    missionManager = new MissionManager();
    buzzerManager = new BuzzerManager(buzPin);
}

SoneeBot::~SoneeBot()
{
    delete touch1;
    delete touch2;
    delete touch3;
    delete servoController;
    delete servoAsync;
    delete displayManager;
    delete missionManager;
    delete buzzerManager;
}

void SoneeBot::init()
{
    Serial.begin(9600);

    // 모든 모듈 초기화
    touch1->init();
    touch2->init();
    touch3->init();
    servoController->init();
    displayManager->init();
    buzzerManager->init();

    // LED 핀 설정
    pinMode(LED_BUILTIN, OUTPUT);

    // 초기화 완료 효과
    buzzerManager->beepPattern(2, 100, 100);

    delay(2000);

    Serial.println("=== SoneeBot Initialized ===");
    Serial.println("Touch Logic: LOW=NotTouched(1), HIGH=Touched(0)");
    printPinInfo();
}

void SoneeBot::update(unsigned long currentMillis)
{
    _currentMillis = currentMillis;
    // 모든 터치 센서 업데이트
    touch1->update();
    touch2->update();
    touch3->update();

    // 터치 상태 업데이트
    updateTouchStates();

    // 서보 애니메이션 업데이트
    servoAsync->update();

    // 부저 업데이트
    buzzerManager->update();

    // 메시지 및 상태 업데이트
    updateMessage();

    // 디스플레이 업데이트
    displayManager->update();

    // 미션 매니저 업데이트
    missionManager->update(touch2->getLastBeepCount(), touch1->getLastBeepCount(),
                           touch1->isHeld(), touch2->isHeld());
}

void SoneeBot::updateTouchStates()
{
    // Touch 1 처리 (미션 감소용)
    if (touch1->isPressed())
    {
        displayManager->showGoodJobMessage();
        Serial.println("Touch 1 PRESSED - Good Job message displayed");
    }

    if (touch1->isHeld())
    {
        Serial.println("Touch 1 HELD - Duration: " + String(touch1->getDuration()) + " ms");
        int expectedBeepCount = (touch1->getDuration() / 500) + 1;
        int currentBeepCount = touch1->getBeepCount();

        // 디버깅 로그 추가
        Serial.println("Expected: " + String(expectedBeepCount) + ", Current: " + String(currentBeepCount));

        if (expectedBeepCount > currentBeepCount)
        {
            servoAsync->startMissionDecraseMotion();
            // 비동기 beep (짧은 소리)
            buzzerManager->beepPattern(2, 100, 100);
            Serial.println("Touch 1 (DECREASE) Beep #" + String(expectedBeepCount) + " - Servo motion executed");

            // TouchSensor의 beepCount를 증가시킴
            touch1->incrementBeepCount();
        }
    }

    if (touch1->isReleased())
    {
        touch1->addToLastBeepCount();
        Serial.println("Touch 1 RELEASED - Total decrease: " + String(touch1->getLastBeepCount()));
    }

    // Touch 2 처리 (미션 증가용)
    if (touch2->isHeld())
    {
        int expectedBeepCount = (touch2->getDuration() / 500) + 1;
        int currentBeepCount = touch2->getBeepCount();

        if (expectedBeepCount > currentBeepCount)
        {
            // 비동기 beep (긴 소리)
            buzzerManager->beepPattern(2, 200, 100);
            Serial.println("Touch 2 (INCREASE) Beep #" + String(expectedBeepCount));
            touch2->incrementBeepCount();
        }
    }

    if (touch2->isReleased())
    {
        touch2->addToLastBeepCount();
        Serial.println("Touch 2 RELEASED - Total increase: " + String(touch2->getLastBeepCount()));
    }

    // Touch 3 처리 (랜덤 서보 선택)
    if (touch3->isHeld())
    {
        int expectedBeepCount = (touch3->getDuration() / 500) + 1;
        int currentBeepCount = touch3->getBeepCount();

        if (expectedBeepCount > currentBeepCount)
        {
            int selectedServo = random(1, 3);
            servoAsync->startRandomMotion(selectedServo);
            // 비동기 beep (중간 소리)
            buzzerManager->beepPattern(2, 150, 100);
            Serial.println("Touch 3 Beep #" + String(expectedBeepCount) + " - Random servo" + String(selectedServo) + " motion executed");
            touch3->incrementBeepCount();
        }
    }

    if (touch3->isReleased())
    {
        touch3->addToLastBeepCount();
        Serial.println("Touch 3 RELEASED - Total beeps: " + String(touch3->getLastBeepCount()));
    }
}

void SoneeBot::updateMessage()
{
    // 미션 완료 효과가 실행 중이면 리턴
    if (displayManager->isMissionCompleteEffectRunning())
    {
        return;
    }

    // Good Job 메시지 표시 중이면 리턴
    if (displayManager->isShowingGoodJob())
    {
        return;
    }

    // 미션 완료 체크
    if (missionManager->isMissionCompleted())
    {
        displayManager->startMissionCompleteEffect();
        servoAsync->startMissionCompleteAnimation();

        // 비동기 완료 사운드
        buzzerManager->beepPattern(3, 200, 100);

        Serial.println("=== MISSION COMPLETED ===");
        missionManager->resetMissionCompleted();
        return;
    }

    // 현재 표시할 미션 카운트 계산
    int displayCount = missionManager->getDisplayCount(touch2->getBeepCount(), touch1->getBeepCount(),
                                                       touch1->isHeld(), touch2->isHeld());

    // 미션 디스플레이 업데이트
    displayManager->updateMissionDisplay(displayCount, touch1->isHeld(), touch2->isHeld());
}

void SoneeBot::testAllDevices()
{
    Serial.println("Testing all devices...");
    displayManager->lcdPrint(0, 0, "Testing All");
    displayManager->lcdPrint(0, 1, "Devices...");

    // 서보 테스트
    servoController->moveServo1(0);
    servoController->moveServo2(180);
    delay(500);
    servoController->moveServo1(180);
    servoController->moveServo2(0);
    delay(500);
    servoController->resetToDefault();

    // 네오픽셀 테스트
    displayManager->fillColor(255, 0, 0);
    delay(300);
    displayManager->fillColor(0, 255, 0);
    delay(300);
    displayManager->fillColor(0, 0, 255);
    delay(300);
    displayManager->clearPixels();

    // 비동기 부저 테스트
    buzzerManager->beepPattern(3, 100, 100);
    Serial.println("All tests completed!");
}

void SoneeBot::printPinInfo()
{
    Serial.println("Pin Configuration:");
    Serial.println("Servo1: 10, Servo2: 11");
    Serial.println("NeoPixel: 3 (Count: 4)");
    Serial.println("Touch1: 8, Touch2: 7, Touch3: 4");
    Serial.println("Buzzer: 2");
    Serial.println("LCD: A4(SDA), A5(SCL)");
}