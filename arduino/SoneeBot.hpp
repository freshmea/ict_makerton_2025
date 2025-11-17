#ifndef SONEEBOT_H
#define SONEEBOT_H

#include "DisplayManager.hpp"
#include "MissionManager.hpp"
#include "ServoController.hpp"
#include "TouchSensor.hpp"
#include <Arduino.h>

class SoneeBot
{
private:
    // 핀 정보
    int buzzerPin;

    // 컴포넌트 객체들
    TouchSensor *touch1;
    TouchSensor *touch2;
    TouchSensor *touch3;
    ServoController *servoController;
    DisplayManager *displayManager;
    MissionManager *missionManager;

public:
    // 생성자
    SoneeBot(int s1Pin = 10, int s2Pin = 11, int neoPin = 3, int neoCount = 8,
             int t1Pin = 8, int t2Pin = 7, int t3Pin = 4, int buzPin = 2);

    // 소멸자
    ~SoneeBot();

    // 초기화 함수
    void init();

    // 메인 업데이트 함수
    void update();

    // 부저 제어
    void beep(int duration = 200);
    void beepPattern(int count = 1, int duration = 200, int interval = 200);

    // 상태 정보
    void printPinInfo();

    // 테스트 함수들
    void testAllDevices();
    void testServos();
    void testNeoPixels();
    void testLCD();
    void testSensors();

    // 컴포넌트 접근자
    TouchSensor *getTouch1() { return touch1; }
    TouchSensor *getTouch2() { return touch2; }
    TouchSensor *getTouch3() { return touch3; }
    ServoController *getServoController() { return servoController; }
    DisplayManager *getDisplayManager() { return displayManager; }
    MissionManager *getMissionManager() { return missionManager; }
};

#endif
