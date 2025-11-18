#ifndef TOUCHSENSOR_HPP
#define TOUCHSENSOR_HPP

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
    TouchSensor(int pinNumber);
    void init();
    void update();
    bool isPressed();
    bool isReleased();
    bool isHeld();
    unsigned long getDuration();
    bool isLongPress(unsigned long threshold = 1000);
    int getBeepCount();
    int getLastBeepCount();
    void resetBeepCount();
    void addToLastBeepCount();
    void incrementBeepCount(); // 새로운 메서드 추가
};

#endif
