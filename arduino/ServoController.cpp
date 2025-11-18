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

void ServoController::moveBothServos(int angle1, int angle2)
{
    moveServo1(angle1);
    moveServo2(angle2);
}

void ServoController::moveServoSmooth(int servoNum, int angle, int delayMs)
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
    moveBothServos(30, 150);
}
