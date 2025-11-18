#include "DisplayManager.hpp"

DisplayManager::DisplayManager(int neoPin, int neoCount)
{
    neoPixelPin = neoPin;
    neoPixelCount = neoCount;
    lcdBacklight = true;

    // LCD 최적화 변수 초기화
    lastServo1Angle = -1;
    lastServo2Angle = -1;
    lastTouch1Display = false;
    lastTouch2Display = false;
    lastTouch3Display = false;
    lastTouch1DurationDisplay = 0;
    lastTouch2DurationDisplay = 0;
    lastTouch3DurationDisplay = 0;
    lastMissionCountDisplay = -1;

    // 미션 완료 효과 변수 초기화
    missionCompleted = false;
    missionCompleteTime = 0;
    showingGoodJob = false;
    goodJobStartTime = 0;

    // 객체 생성
    strip = new Adafruit_NeoPixel(neoPixelCount, neoPixelPin, NEO_GRB + NEO_KHZ800);
    lcd = new LiquidCrystal_I2C(0x27, 16, 2);
}

DisplayManager::~DisplayManager()
{
    delete strip;
    delete lcd;
}

void DisplayManager::init()
{
    // 네오픽셀 초기화
    strip->begin();
    strip->setBrightness(50);
    strip->show();

    // LCD 초기화
    lcd->begin();
    lcd->backlight();

    // 초기화 메시지
    lcdClear();
    lcdPrint(0, 0, "SoneeBot Ready!");
    lcdPrint(0, 1, "Initializing...");

    // 초기화 효과
    rainbowEffect();
}

void DisplayManager::lcdPrint(int col, int row, String text)
{
    lcd->setCursor(col, row);
    lcd->print(text);
}

void DisplayManager::lcdClear()
{
    lcd->clear();
}

void DisplayManager::lcdBacklightOn()
{
    lcd->backlight();
    lcdBacklight = true;
}

void DisplayManager::lcdBacklightOff()
{
    lcd->noBacklight();
    lcdBacklight = false;
}

void DisplayManager::showGoodJobMessage()
{
    showingGoodJob = true;
    goodJobStartTime = millis();
    lcdClear();
    lcdPrint(0, 0, "Good Job !!");
}

void DisplayManager::updateStatusDisplay(int servo1Angle, int servo2Angle, bool touch1, bool touch2, bool touch3,
                                         unsigned long touch1Duration, unsigned long touch2Duration, unsigned long touch3Duration)
{
    // ...existing code... (기존 updateStatusDisplay 로직)
    bool needUpdate = false;

    if (servo1Angle != lastServo1Angle || servo2Angle != lastServo2Angle)
    {
        needUpdate = true;
    }

    if (touch1 != lastTouch1Display || touch2 != lastTouch2Display || touch3 != lastTouch3Display)
    {
        needUpdate = true;
    }

    unsigned long currentTouch1Sec = touch1Duration / 1000;
    unsigned long currentTouch2Sec = touch2Duration / 1000;
    unsigned long currentTouch3Sec = touch3Duration / 1000;

    if (currentTouch1Sec != lastTouch1DurationDisplay ||
        currentTouch2Sec != lastTouch2DurationDisplay ||
        currentTouch3Sec != lastTouch3DurationDisplay)
    {
        needUpdate = true;
    }

    if (needUpdate)
    {
        lcdClear();
        lcdPrint(0, 0, "S1:" + String(servo1Angle) + " S2:" + String(servo2Angle));

        String touchInfo = "T:";
        touchInfo += touch1 ? "0" : "1";
        touchInfo += touch2 ? "0" : "1";
        touchInfo += touch3 ? "0" : "1";

        if (touch1 || touch2 || touch3)
        {
            touchInfo += " ";
            if (touch1)
                touchInfo += String(currentTouch1Sec) + "s ";
            if (touch2)
                touchInfo += String(currentTouch2Sec) + "s ";
            if (touch3)
                touchInfo += String(currentTouch3Sec) + "s";
        }

        lcdPrint(0, 1, touchInfo);

        // 이전 상태 업데이트
        lastServo1Angle = servo1Angle;
        lastServo2Angle = servo2Angle;
        lastTouch1Display = touch1;
        lastTouch2Display = touch2;
        lastTouch3Display = touch3;
        lastTouch1DurationDisplay = currentTouch1Sec;
        lastTouch2DurationDisplay = currentTouch2Sec;
        lastTouch3DurationDisplay = currentTouch3Sec;
    }
}

void DisplayManager::setPixelColor(int pixel, uint8_t r, uint8_t g, uint8_t b)
{
    if (pixel >= 0 && pixel < neoPixelCount)
    {
        strip->setPixelColor(pixel, strip->Color(r, g, b));
        strip->show();
    }
}

void DisplayManager::fillColor(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < neoPixelCount; i++)
    {
        strip->setPixelColor(i, strip->Color(r, g, b));
    }
    strip->show();
}

void DisplayManager::clearPixels()
{
    fillColor(0, 0, 0);
}

void DisplayManager::rainbowEffect()
{
    for (int j = 0; j < 256; j += 5)
    {
        for (int i = 0; i < neoPixelCount; i++)
        {
            strip->setPixelColor(i, strip->ColorHSV((i * 65536L / neoPixelCount) + j * 256));
        }
        strip->show();
        delay(10);
    }
}

void DisplayManager::updateMissionPixels(int missionCount)
{
    clearPixels();

    if (missionCount <= 0)
        return;

    for (int i = 0; i < min(missionCount, 12); i++)
    {
        int pixelIndex = i % 4;

        if (i < 4)
        {
            strip->setPixelColor(pixelIndex, strip->Color(0, 255, 0));
        }
        else if (i < 8)
        {
            strip->setPixelColor(pixelIndex, strip->Color(0, 0, 255));
        }
        else
        {
            strip->setPixelColor(pixelIndex, strip->Color(255, 0, 0));
        }
    }

    strip->show();
}

void DisplayManager::missionCompleteEffect()
{
    unsigned long elapsed = millis() - missionCompleteTime;

    if (elapsed < 3000)
    {
        int hue = (elapsed / 10) % 360;
        for (int i = 0; i < neoPixelCount; i++)
        {
            int pixelHue = (hue + i * 90) % 360;
            strip->setPixelColor(i, strip->ColorHSV(pixelHue * 182));
        }
        strip->show();
    }
    else
    {
        missionCompleted = false;
        clearPixels();
    }
}

void DisplayManager::updateMissionDisplay(int missionCount, bool touch1State, bool touch2State)
{
    if (missionCount != lastMissionCountDisplay || touch1State != lastTouch1Display || touch2State != lastTouch2Display)
    {
        lcdClear();
        lcdPrint(0, 0, "Today Mission!!");

        String missionInfo = ": " + String(missionCount);
        lcdPrint(0, 1, missionInfo);

        if (touch2State)
        {
            lcdPrint(4, 1, " (+ing...)");
        }
        else if (touch1State)
        {
            lcdPrint(4, 1, " (-ing...)");
        }
        else
        {
            lcdPrint(4, 1, " (Done)");
        }

        updateMissionPixels(missionCount);

        lastMissionCountDisplay = missionCount;
        lastTouch1Display = touch1State;
        lastTouch2Display = touch2State;
    }
}

void DisplayManager::startMissionCompleteEffect()
{
    missionCompleted = true;
    missionCompleteTime = millis();

    lcdClear();
    lcdPrint(0, 0, "MISSION");
    lcdPrint(0, 1, "COMPLETED!");
}

void DisplayManager::update()
{
    if (showingGoodJob)
    {
        unsigned long elapsed = millis() - goodJobStartTime;
        if (elapsed >= 1000)
        {
            showingGoodJob = false;
        }
    }

    if (missionCompleted)
    {
        missionCompleteEffect();
    }
}

bool DisplayManager::isMissionCompleteEffectRunning()
{
    return missionCompleted;
}

bool DisplayManager::isShowingGoodJob()
{
    return showingGoodJob;
}