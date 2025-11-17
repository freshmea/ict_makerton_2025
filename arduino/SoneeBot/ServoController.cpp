#include "ServoController.hpp"

ServoController::ServoController(int s1Pin, int s2Pin)
    : servo1Async(s1Pin), servo2Async(s2Pin) // 멤버 초기화 리스트 사용
{
    servo1Pin = s1Pin;
    servo2Pin = s2Pin;

    Serial.print("ServoController created, Free RAM: ");
    Serial.println(getFreeMemory());
}

ServoController::~ServoController()
{
    // 직접 객체이므로 delete 불필요
}

// 메모리 체크 함수 추가
int ServoController::getFreeMemory()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void ServoController::init()
{
    // 초기 각도 설정
    servo1Async.moveToAngle(30);  // servo1 초기 각도
    servo2Async.moveToAngle(150); // servo2 초기 각도

    servo1Async.init();
    servo2Async.init();
}

void ServoController::update(unsigned long currentMillis)
{
    servo1Async.update(currentMillis);
    servo2Async.update(currentMillis);
}

void ServoController::executeRandomMotion(unsigned long currentMillis)
{
    // currentMillis가 0이면 현재 시간을 가져옴 (호환성을 위해)
    if (currentMillis == 0)
    {
        currentMillis = millis();
    }

    int selectedServo = random(1, 3);

    if (selectedServo == 1)
    {
        // 서보1: 90도로 이동 후 원래 위치로 복귀
        servo1Async.addToQueue(currentMillis, 90);
        servo1Async.addToQueue(currentMillis + 500, 30);
        Serial.println("Random motion queued - Servo1 to 90° then back to 30°");
    }
    else
    {
        // 서보2: 90도로 이동 후 원래 위치로 복귀
        servo2Async.addToQueue(currentMillis, 90);
        servo2Async.addToQueue(currentMillis + 500, 150);
        Serial.println("Random motion queued - Servo2 to 90° then back to 150°");
    }
}

void ServoController::executeMissionDecrease(unsigned long currentMillis)
{
    // currentMillis가 0이면 현재 시간을 가져옴 (호환성을 위해)
    if (currentMillis == 0)
    {
        currentMillis = millis();
    }

    // 1단계: 양쪽 서보를 90도로 이동 (500ms 동안)
    for (int i = 0; i <= 5; i++)
    {
        servo1Async.addToQueue(currentMillis + (i * 100), 90);
        servo2Async.addToQueue(currentMillis + (i * 100), 90);
    }

    // 2단계: 원래 위치로 복귀 (500ms 동안, 300ms 후 시작)
    for (int i = 0; i <= 4; i++)
    {
        servo1Async.addToQueue(currentMillis + 800 + (i * 100), 30);
        servo2Async.addToQueue(currentMillis + 800 + (i * 100), 150);
    }

    Serial.println("Mission decrease motion queued - Both servos to 90° then back to original positions");
}