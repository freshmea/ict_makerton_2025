#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include "ServoAsync.hpp"
#include <Arduino.h>

class ServoController
{
private:
    ServoAsync *servo1Async;
    ServoAsync *servo2Async;
    int servo1Pin;
    int servo2Pin;

public:
    ServoController(int s1Pin, int s2Pin);
    ~ServoController();

    void init();
    void update(unsigned long currentMillis);

    void moveServo1(int angle);
    void moveServo2(int angle);
    void moveServoSmooth(int servoNum, int angle, int durationMs = 1000, unsigned long currentMillis = 0);
    void executeRandomMotion(unsigned long currentMillis = 0);
    void executeMissionDecrease(unsigned long currentMillis = 0);

    int getServo1Angle();
    int getServo2Angle();

    // 디버깅용 함수들
    int getServo1QueueSize();
    int getServo2QueueSize();
};

#endif
