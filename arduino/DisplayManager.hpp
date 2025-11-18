#ifndef DISPLAYMANAGER_HPP
#define DISPLAYMANAGER_HPP

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class DisplayManager
{
private:
    Adafruit_NeoPixel *strip;
    LiquidCrystal_I2C *lcd;
    int neoPixelPin;
    int neoPixelCount;
    bool lcdBacklight;

    // LCD 최적화 변수들
    int lastServo1Angle;
    int lastServo2Angle;
    bool lastTouch1Display;
    bool lastTouch2Display;
    bool lastTouch3Display;
    unsigned long lastTouch1DurationDisplay;
    unsigned long lastTouch2DurationDisplay;
    unsigned long lastTouch3DurationDisplay;
    int lastMissionCountDisplay;

    // 미션 완료 효과 변수들
    bool missionCompleted;
    unsigned long missionCompleteTime;
    bool showingGoodJob;
    unsigned long goodJobStartTime;

public:
    DisplayManager(int neoPin = 3, int neoCount = 4);
    ~DisplayManager();
    void init();

    // LCD 제어
    void lcdPrint(int col, int row, String text);
    void lcdClear();
    void lcdBacklightOn();
    void lcdBacklightOff();
    void showGoodJobMessage();
    void updateStatusDisplay(int servo1Angle, int servo2Angle, bool touch1, bool touch2, bool touch3,
                             unsigned long touch1Duration, unsigned long touch2Duration, unsigned long touch3Duration);

    // 네오픽셀 제어
    void setPixelColor(int pixel, uint8_t r, uint8_t g, uint8_t b);
    void fillColor(uint8_t r, uint8_t g, uint8_t b);
    void clearPixels();
    void rainbowEffect();
    void updateMissionPixels(int missionCount);
    void missionCompleteEffect();

    // 미션 관련 디스플레이
    void updateMissionDisplay(int missionCount, bool touch1State, bool touch2State);
    void startMissionCompleteEffect();
    void update();
    bool isMissionCompleteEffectRunning();
    bool isShowingGoodJob();
};

#endif
