#ifndef SERVOASYNC_HPP
#define SERVOASYNC_HPP

#include "ServoController.hpp"
#include <Arduino.h>

class ServoAsync
{
private:
    ServoController *servoController;
    bool isAnimating;
    unsigned long animationStartTime;
    unsigned long animationDuration;
    int animationType;

    // Random motion용 변수들 추가
    int randomServoNum;
    int originalAngle1;
    int originalAngle2;

public:
    ServoAsync(ServoController *controller);
    void update();
    void startMissionCompleteAnimation();
    void startRandomMotion(int servoNum);
    void startMissionDecraseMotion();
    bool isAnimationRunning();
};

#endif
