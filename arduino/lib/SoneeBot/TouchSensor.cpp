#include "TouchSensor.hpp"

TouchSensor::TouchSensor(int sensorPin)
{
    pin = sensorPin;
    currentState = false;
    lastState = false;
    startTime = 0;
    beepCount = 0;
    lastBeepCount = 0;
}

void TouchSensor::init()
{
    pinMode(pin, INPUT);
}

void TouchSensor::update()
{
    unsigned long currentTime = millis();

    lastState = currentState;
    currentState = digitalRead(pin) == HIGH;

    if (currentState)
    {
        if (!lastState)
        {
            startTime = currentTime;
            beepCount = 0;
        }

        unsigned long duration = currentTime - startTime;
        int expectedBeepCount = (duration / 500) + 1;
        if (expectedBeepCount > beepCount)
        {
            beepCount++;
        }
    }
    else
    {
        if (lastState)
        {
            lastBeepCount += beepCount;
        }
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

bool TouchSensor::isLongPress(unsigned long threshold)
{
    return isHeld() && getDuration() >= threshold;
}

unsigned long TouchSensor::getDuration()
{
    if (currentState && startTime > 0)
    {
        return millis() - startTime;
    }
    return 0;
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
    lastBeepCount = 0;
}

bool TouchSensor::getCurrentState()
{
    return currentState;
}

bool TouchSensor::getLastState()
{
    return lastState;
}
