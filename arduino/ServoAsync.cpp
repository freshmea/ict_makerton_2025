#include "ServoAsync.hpp"

ServoAsync::ServoAsync(ServoController *controller)
{
    servoController = controller;
    isAnimating = false;
    animationStartTime = 0;
    animationDuration = 0;
    animationType = 0;
}

void ServoAsync::update(unsigned long currentMillis)
{
    if (!isAnimating)
        return;

    unsigned long elapsed = currentMillis - animationStartTime;

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

                servoController->moveServo1(angle1);
                servoController->moveServo2(angle2);
            }
        }
        else
        {
            servoController->resetToDefault();
            isAnimating = false;
        }
    }
    else if (animationType == 2) // Random motion
    {
        if (elapsed >= animationDuration)
        {
            // 애니메이션 시간이 끝나면 원래 위치로 복귀
            servoController->resetToDefault();
            isAnimating = false;
        }
    }
    else if (animationType == 3) // Mission decrease motion
    {
        if (elapsed >= animationDuration)
        {
            // 애니메이션 시간이 끝나면 원래 위치로 복귀
            servoController->resetToDefault();
            isAnimating = false;
        }
    }
}

void ServoAsync::startMissionCompleteAnimation(unsigned long currentMillis)
{
    isAnimating = true;
    animationStartTime = currentMillis;
    animationDuration = 4000;
    animationType = 1;
}

void ServoAsync::startRandomMotion(int servoNum, unsigned long currentMillis)
{
    if (isAnimating)
        return; // 이미 애니메이션 중이면 무시

    // 즉시 90도로 이동
    if (servoNum == 1)
    {
        servoController->moveServo1(90);
    }
    else if (servoNum == 2)
    {
        servoController->moveServo2(90);
    }
    else
    {
        // 잘못된 servoNum이면 둘 다 이동
        servoController->moveServo1(90);
        servoController->moveServo2(90);
    }

    isAnimating = true;
    animationStartTime = currentMillis;
    animationDuration = 450; // 450ms 후 원래 위치로 복귀
    animationType = 2;
}

void ServoAsync::startMissionDecraseMotion(unsigned long currentMillis)
{
    if (isAnimating)
        return; // 이미 애니메이션 중이면 무시

    // 즉시 90도로 이동
    servoController->moveServo1(90);
    servoController->moveServo2(90);

    isAnimating = true;
    animationStartTime = currentMillis;
    animationDuration = 450; // 450ms 후 원래 위치로 복귀
    animationType = 3;
}

bool ServoAsync::isAnimationRunning()
{
    return isAnimating;
}
