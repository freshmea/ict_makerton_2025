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

public:
    ServoAsync(ServoController *controller);
    void update();
    void startMissionCompleteAnimation();
    void startRandomMotion(int servoNum);
    void startMissionDecraseMotion();
    bool isAnimationRunning();
};

#endif
