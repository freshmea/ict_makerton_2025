#include "TouchSensor.hpp"

TouchSensor::TouchSensor(int pinNumber)
{
    pin = pinNumber;
    currentState = false;
    lastState = false;
    startTime = 0;
    duration = 0;
    beepCount = 0;
    lastBeepCount = 0;
    beepTriggered = false;
}

void TouchSensor::init()
{
    pinMode(pin, INPUT);
}

void TouchSensor::update()
{
    lastState = currentState;
    currentState = digitalRead(pin) == HIGH;

    if (currentState)
    {
        if (!lastState)
        {
            startTime = millis();
            beepCount = 0; // 터치 시작 시 beepCount 초기화
        }
        duration = millis() - startTime;

        // beepCount 자동 증가 로직 제거
        // SoneeBot에서 직접 관리하도록 변경
    }
    else
    {
        if (lastState)
        {
            lastBeepCount += beepCount;
        }
        duration = 0;
        startTime = 0;
        beepCount = 0;
    }
}

bool TouchSensor::isPressed()
{
    return currentState && !lastState;
}

bool TouchSensor::isReleased()
{
    return !currentState && lastState;
}

bool TouchSensor::isHeld()
{
    return currentState;
}

unsigned long TouchSensor::getDuration()
{
    return duration;
}

bool TouchSensor::isLongPress(unsigned long threshold)
{
    return isHeld() && getDuration() >= threshold;
}

int TouchSensor::getBeepCount()
{
    return beepCount;
}

int TouchSensor::getLastBeepCount()
{
    return lastBeepCount;
}

void TouchSensor::resetBeepCount()
{
    beepCount = 0;
    lastBeepCount = 0;
}

void TouchSensor::addToLastBeepCount()
{
    lastBeepCount += beepCount;
}

void TouchSensor::incrementBeepCount()
{
    beepCount++;
}
