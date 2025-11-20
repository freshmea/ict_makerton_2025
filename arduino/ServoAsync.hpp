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
    void update(unsigned long currentMillis);
    void startMissionCompleteAnimation(unsigned long currentMillis);
    void startRandomMotion(int servoNum, unsigned long currentMillis);
    void startMissionDecraseMotion(unsigned long currentMillis);
    bool isAnimationRunning();
};

#endif
