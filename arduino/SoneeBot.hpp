#ifndef SONEEBOT_H
#define SONEEBOT_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>

class SoneeBot
{
private:
    // 핀 정보
    int servo1Pin;
    int servo2Pin;
    int neoPixelPin;
    int neoPixelCount;
    int touch1Pin;
    int touch2Pin;
    int touch3Pin;
    int buzzerPin;

    // 객체들
    Servo servo1;
    Servo servo2;
    Adafruit_NeoPixel *strip;
    LiquidCrystal_I2C *lcd;

    // 서보 상태
    int servo1Angle;
    int servo2Angle;

    // LCD 상태
    bool lcdBacklight;

    // 터치 센서 상태 추가
    bool touch1State;
    bool touch2State;
    bool touch3State;
    bool lastTouch1State;
    bool lastTouch2State;
    bool lastTouch3State;

    // 터치 지속 시간 추적
    unsigned long touch1StartTime;
    unsigned long touch2StartTime;
    unsigned long touch3StartTime;
    unsigned long touch1Duration;
    unsigned long touch2Duration;
    unsigned long touch3Duration;

    // 터치별 부저 알림 상태 추가
    bool touch1BeepTriggered;
    bool touch2BeepTriggered;
    bool touch3BeepTriggered;

    // 터치별 부저 카운터 추가 (0.5초마다 증가)
    int touch1BeepCount;
    int touch2BeepCount;
    int touch3BeepCount;

    // 터치 해제 시 마지막 카운트 저장
    int touch1LastBeepCount;
    int touch2LastBeepCount;
    int touch3LastBeepCount;

    // 미션 카운터 추가
    int missionCount;

    // 미션 완료 관련 변수 추가
    bool missionCompleted;
    unsigned long missionCompleteTime;
    int lastMissionCount;

    // LCD 상태 최적화를 위한 이전 값 저장
    int lastServo1Angle;
    int lastServo2Angle;
    bool lastTouch1Display;
    bool lastTouch2Display;
    bool lastTouch3Display;
    unsigned long lastTouch1DurationDisplay;
    unsigned long lastTouch2DurationDisplay;
    unsigned long lastTouch3DurationDisplay;
    int lastTouch3BeepCountDisplay;
    bool lastTouch3StateDisplay;

public:
    // 생성자
    SoneeBot(int s1Pin = 10, int s2Pin = 11, int neoPin = 3, int neoCount = 8,
             int t1Pin = 8, int t2Pin = 7, int t3Pin = 4, int buzPin = 2);

    // 소멸자
    ~SoneeBot();

    // 초기화 함수
    void init();

    // 서보 제어
    void moveServo1(int angle);
    void moveServo2(int angle);
    void moveBothServos(int angle1, int angle2);
    void moveServoSmooth(int servoNum, int angle, int delayMs = 15);

    // 네오픽셀 제어
    void setPixelColor(int pixel, uint8_t r, uint8_t g, uint8_t b);
    void fillColor(uint8_t r, uint8_t g, uint8_t b);
    void clearPixels();
    void rainbowEffect();
    void updateMissionPixels(int missionCount); // 미션 카운트에 따른 네오픽셀 업데이트 추가
    void missionCompleteEffect();               // 미션 완료 효과 추가

    // LCD 제어
    void lcdPrint(int col, int row, String text);
    void lcdClear();
    void lcdBacklightOn();
    void lcdBacklightOff();
    void updateStatusDisplay();
    void updateMessage(); // 새로운 함수 추가

    // 센서 읽기 (기존 함수들 유지)
    bool readTouch1();
    bool readTouch2();
    bool readTouch3();

    // 새로운 터치 상태 함수들 추가
    void updateTouchStates();
    bool isTouchPressed(int touchNum);
    bool isTouchReleased(int touchNum);
    bool isTouchHeld(int touchNum);
    unsigned long getTouchDuration(int touchNum);
    bool isTouchLongPress(int touchNum, unsigned long threshold = 1000);

    // 부저 제어
    void beep(int duration = 200);
    void beepPattern(int count = 1, int duration = 200, int interval = 200);

    // 상태 정보
    int getServo1Angle();
    int getServo2Angle();
    void printPinInfo();

    // 테스트 함수들
    void testAllDevices();
    void testServos();
    void testNeoPixels();
    void testLCD();
    void testSensors();
};

#endif
