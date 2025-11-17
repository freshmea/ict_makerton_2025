#include "DisplayManager.hpp"

DisplayManager::DisplayManager(int neoPin, int neoCount)
{
    neoPixelPin = neoPin;
    neoPixelCount = 4; // 고정값으로 단순화
    lcdBacklight = true;

    // 상태 변수 최소한만 초기화
    lastMissionCountDisplay = -1;
    lastTouch1Display = false;
    lastTouch2Display = false;

    Serial.print("DisplayManager created (static allocation), Free RAM: ");
    Serial.println(getFreeMemory());

    // 네오픽셀을 더 작은 설정으로 - 정적 할당으로 안전하게 생성
    strip = new Adafruit_NeoPixel(4, neoPixelPin, NEO_GRB + NEO_KHZ800);

    // LCD 객체 생성
    lcd = new LiquidCrystal_I2C(0x27, 16, 2);

    Serial.print("DisplayManager initialization completed, Free RAM: ");
    Serial.println(getFreeMemory());
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
}

// LCD 관련 함수들
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

// 네오픽셀 관련 함수들
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

void DisplayManager::missionCompleteEffect(unsigned long elapsed)
{
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
}

void DisplayManager::showGoodJobMessage()
{
    lcdClear();
    lcdPrint(0, 0, "Good Job !!");
}

void DisplayManager::showMissionCompleteMessage()
{
    lcdClear();
    lcdPrint(0, 0, "MISSION");
    lcdPrint(0, 1, "COMPLETED!");
}

void DisplayManager::updateMissionDisplay(int missionCount, bool touch1State, bool touch2State, int touch1BeepCount, int touch2BeepCount)
{
    // 단순화된 업데이트 로직
    if (missionCount != lastMissionCountDisplay ||
        touch1State != lastTouch1Display ||
        touch2State != lastTouch2Display)
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

        updateMissionPixels(missionCount);

        lastMissionCountDisplay = missionCount;
        lastTouch1Display = touch1State;
        lastTouch2Display = touch2State;
    }
}

// updateStatusDisplay 함수를 단순화하거나 제거
void DisplayManager::updateStatusDisplay(int servo1Angle, int servo2Angle, bool touch1State, bool touch2State, bool touch3State, unsigned long touch1Duration, unsigned long touch2Duration, unsigned long touch3Duration)
{
    // 메모리 절약을 위해 단순화된 버전
    lcdClear();
    lcdPrint(0, 0, "S1:" + String(servo1Angle) + " S2:" + String(servo2Angle));

    String touchInfo = "T:";
    touchInfo += touch1State ? "0" : "1";
    touchInfo += touch2State ? "0" : "1";
    touchInfo += touch3State ? "0" : "1";
    lcdPrint(0, 1, touchInfo);
}

// 메모리 체크 함수 추가
int DisplayManager::getFreeMemory()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}