#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include "ServoAsync.hpp"
#include <Arduino.h>

class ServoController
{
public:
    ServoAsync servo1Async; // 포인터 → 직접 객체로 변경
    ServoAsync servo2Async; // 포인터 → 직접 객체로 변경
    int servo1Pin;
    int servo2Pin;

    ServoController(int s1Pin, int s2Pin);
    ~ServoController();

    void init();
    void update(unsigned long currentMillis);
    void executeRandomMotion(unsigned long currentMillis = 0);
    void executeMissionDecrease(unsigned long currentMillis = 0);

private:
    // 메모리 체크 함수 추가
    int getFreeMemory();
};

#endif
