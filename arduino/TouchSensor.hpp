#ifndef TOUCHSENSOR_H
#define TOUCHSENSOR_H

#include <Arduino.h>

class TouchSensor
{
private:
    int pin;
    bool currentState;
    bool lastState;
    unsigned long startTime;
    unsigned long duration;
    int beepCount;
    int lastBeepCount;
    bool beepTriggered;

public:
    TouchSensor(int sensorPin);

    void init();
    void update();

    bool isPressed();
    bool isReleased();
    bool isHeld();
    bool isLongPress(unsigned long threshold = 1000);

    unsigned long getDuration();
    int getBeepCount();
    int getLastBeepCount();
    void resetBeepCount();

    bool getCurrentState();
    bool getLastState();
};

#endif
