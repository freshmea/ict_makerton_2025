#ifndef SONEEBOT_HPP
#define SONEEBOT_HPP

#include "DisplayManager.hpp"
#include "MissionManager.hpp"
#include "PassiveBuzzerManager.hpp"
#include "ServoAsync.hpp"
#include "ServoController.hpp"
#include "TouchSensor.hpp"
#include <Arduino.h>

class SoneeBot
{
private:
    // 모듈들
    TouchSensor *touch1;
    TouchSensor *touch2;
    TouchSensor *touch3;
    ServoController *servoController;
    ServoAsync *servoAsync;
    DisplayManager *displayManager;
    MissionManager *missionManager;
    PassiveBuzzerManager *buzzerManager;
    int missionCount;
    int lastMissionCount;

public:
    SoneeBot(int s1Pin = 10, int s2Pin = 11, int neoPin = 3, int neoCount = 4,
             int t1Pin = 8, int t2Pin = 7, int t3Pin = 4, int buzPin = 2);
    ~SoneeBot();

    void init();
    void updateTouchStates();
    void updateMessage();
    void update(unsigned long currentMillis);
    unsigned long _currentMillis;

    // 테스트 함수들
    void testAllDevices();

    // 접근자 함수들
    TouchSensor *getTouch1() { return touch1; }
    TouchSensor *getTouch2() { return touch2; }
    TouchSensor *getTouch3() { return touch3; }
    ServoController *getServoController() { return servoController; }
    DisplayManager *getDisplayManager() { return displayManager; }
    MissionManager *getMissionManager() { return missionManager; }
    PassiveBuzzerManager *getBuzzerManager() { return buzzerManager; }
};

#endif
