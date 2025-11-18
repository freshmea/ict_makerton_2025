#ifndef SERVOCONTROLLER_HPP
#define SERVOCONTROLLER_HPP

#include <Arduino.h>
#include <Servo.h>

class ServoController
{
private:
    Servo servo1;
    Servo servo2;
    int servo1Pin;
    int servo2Pin;
    int servo1Angle;
    int servo2Angle;

public:
    ServoController(int s1Pin = 10, int s2Pin = 11);
    void init();
    void moveServo1(int angle);
    void moveServo2(int angle);
    void moveBothServos(int angle1, int angle2);
    void moveServoSmooth(int servoNum, int angle, int delayMs = 15);
    int getServo1Angle();
    int getServo2Angle();
    void resetToDefault();
};

#endif
