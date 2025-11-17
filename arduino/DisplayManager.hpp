#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class DisplayManager
{
private:
    LiquidCrystal_I2C *lcd;
    Adafruit_NeoPixel *strip;
    int neoPixelPin;
    int neoPixelCount;
    bool lcdBacklight;

    // LCD 상태 최적화 변수들
    int lastServo1Angle;
    int lastServo2Angle;
    bool lastTouch1Display;
    bool lastTouch2Display;
    bool lastTouch3Display;
    unsigned long lastTouch1DurationDisplay;
    unsigned long lastTouch2DurationDisplay;
    unsigned long lastTouch3DurationDisplay;
    int lastMissionCountDisplay;
    bool lastTouch3StateDisplay;

public:
    DisplayManager(int neoPin, int neoCount);
    ~DisplayManager();

    void init();

    // LCD 제어
    void lcdPrint(int col, int row, String text);
    void lcdClear();
    void lcdBacklightOn();
    void lcdBacklightOff();
    void updateStatusDisplay(int servo1Angle, int servo2Angle, bool touch1State, bool touch2State, bool touch3State, unsigned long touch1Duration, unsigned long touch2Duration, unsigned long touch3Duration);
    void showGoodJobMessage();
    void showMissionCompleteMessage();
    void updateMissionDisplay(int missionCount, bool touch1State, bool touch2State, int touch1BeepCount, int touch2BeepCount);

    // 네오픽셀 제어
    void setPixelColor(int pixel, uint8_t r, uint8_t g, uint8_t b);
    void fillColor(uint8_t r, uint8_t g, uint8_t b);
    void clearPixels();
    void rainbowEffect();
    void updateMissionPixels(int missionCount);
    void missionCompleteEffect(unsigned long elapsed);
};

#endif
