#include "ServoController.hpp"

ServoController::ServoController(int s1Pin, int s2Pin)
{
    servo1Pin = s1Pin;
    servo2Pin = s2Pin;
    servo1Angle = 30;
    servo2Angle = 150;
}

void ServoController::init()
{
    servo1.attach(servo1Pin);
    servo2.attach(servo2Pin);
    servo1.write(servo1Angle);
    servo2.write(servo2Angle);
}

void ServoController::moveServo1(int angle)
{
    angle = constrain(angle, 0, 180);
    servo1.write(angle);
    servo1Angle = angle;
}

void ServoController::moveServo2(int angle)
{
    angle = constrain(angle, 0, 180);
    servo2.write(angle);
    servo2Angle = angle;
}

int ServoController::getServo1Angle()
{
    return servo1Angle;
}

int ServoController::getServo2Angle()
{
    return servo2Angle;
}

void ServoController::resetToDefault()
{
    moveServo1(30);
    moveServo2(150);
}
