#ifndef BUZZERMANAGER_HPP
#define BUZZERMANAGER_HPP

#include <Arduino.h>

class BuzzerManager
{
private:
    int buzzerPin;
    bool playing; // isPlaying -> playing으로 변경
    unsigned long startTime;
    unsigned long duration;
    unsigned long pauseDuration;
    int repeatCount;
    int currentRepeat;
    bool isPaused;
    unsigned long pauseStartTime;

public:
    BuzzerManager(int pin = 2);
    void init();
    void update();

    // 단일 beep
    void beep(int duration = 200);

    // 패턴 beep (비동기)
    void beepPattern(int count = 1, int duration = 200, int interval = 200);

    // 재생 중인지 확인
    bool isPlaying();

    // 재생 중지
    void stop();
};

#endif
