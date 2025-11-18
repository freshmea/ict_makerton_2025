#include "ServoAsync.hpp"

ServoAsync::ServoAsync(ServoController *controller)
{
    servoController = controller;
    isAnimating = false;
    animationStartTime = 0;
    animationDuration = 0;
    animationType = 0;
}

void ServoAsync::update()
{
    if (!isAnimating)
        return;

    unsigned long elapsed = millis() - animationStartTime;

    if (animationType == 1) // Mission complete animation
    {
        if (elapsed < animationDuration)
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

                servoController->moveBothServos(angle1, angle2);
            }
        }
        else
        {
            isAnimating = false;
            servoController->resetToDefault();
        }
    }
    else if (animationType == 2) // Random motion
    {
        // Random motion is handled instantly, no update needed
        isAnimating = false;
    }
    else if (animationType == 3) // Mission decrease motion
    {
        // Mission decrease motion is handled instantly, no update needed
        isAnimating = false;
    }
}

void ServoAsync::startMissionCompleteAnimation()
{
    isAnimating = true;
    animationStartTime = millis();
    animationDuration = 3000;
    animationType = 1;
}

void ServoAsync::startRandomMotion(int servoNum)
{
    int originalAngle1 = servoController->getServo1Angle();
    int originalAngle2 = servoController->getServo2Angle();

    if (servoNum == 1)
    {
        servoController->moveServo1(90);
        delay(500);
        servoController->moveServo1(originalAngle1);
    }
    else
    {
        servoController->moveServo2(90);
        delay(500);
        servoController->moveServo2(originalAngle2);
    }

    animationType = 2;
}

void ServoAsync::startMissionDecraseMotion()
{
    servoController->moveBothServos(90, 90);
    delay(300);
    servoController->moveBothServos(30, 150);

    animationType = 3;
}

bool ServoAsync::isAnimationRunning()
{
    return isAnimating;
}
