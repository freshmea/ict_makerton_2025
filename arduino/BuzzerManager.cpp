#include "BuzzerManager.hpp"

BuzzerManager::BuzzerManager(int pin)
{
    buzzerPin = pin;
    playing = false; // isPlaying -> playing으로 변경
    startTime = 0;
    duration = 0;
    pauseDuration = 0;
    repeatCount = 0;
    currentRepeat = 0;
    isPaused = false;
    pauseStartTime = 0;
}

void BuzzerManager::init()
{
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
}

void BuzzerManager::update()
{
    if (!playing)
        return; // isPlaying -> playing으로 변경

    unsigned long currentTime = millis();

    if (isPaused)
    {
        // 일시정지 상태
        if (currentTime - pauseStartTime >= pauseDuration)
        {
            isPaused = false;
            currentRepeat++;

            if (currentRepeat >= repeatCount)
            {
                // 모든 반복 완료
                playing = false; // isPlaying -> playing으로 변경
                digitalWrite(buzzerPin, LOW);
            }
            else
            {
                // 다음 beep 시작
                startTime = currentTime;
                digitalWrite(buzzerPin, HIGH);
            }
        }
    }
    else
    {
        // beep 재생 상태
        if (currentTime - startTime >= duration)
        {
            digitalWrite(buzzerPin, LOW);

            if (currentRepeat + 1 >= repeatCount)
            {
                // 모든 반복 완료
                playing = false; // isPlaying -> playing으로 변경
            }
            else
            {
                // 일시정지 시작
                isPaused = true;
                pauseStartTime = currentTime;
            }
        }
    }
}

void BuzzerManager::beep(int beepDuration)
{
    // 최소 지속시간 보장
    if (beepDuration < 100)
        beepDuration = 100;

    // 현재 재생 중이면 중지
    stop();

    // 단일 beep 설정
    playing = true; // isPlaying -> playing으로 변경
    startTime = millis();
    duration = beepDuration;
    pauseDuration = 0;
    repeatCount = 1;
    currentRepeat = 0;
    isPaused = false;

    digitalWrite(buzzerPin, HIGH);
}

void BuzzerManager::beepPattern(int count, int beepDuration, int interval)
{
    // 최소 지속시간 보장
    if (beepDuration < 100)
        beepDuration = 100;

    // 현재 재생 중이면 중지
    stop();

    // 패턴 beep 설정
    playing = true; // isPlaying -> playing으로 변경
    startTime = millis();
    duration = beepDuration;
    pauseDuration = interval;
    repeatCount = count;
    currentRepeat = 0;
    isPaused = false;

    digitalWrite(buzzerPin, HIGH);
}

bool BuzzerManager::isPlaying()
{
    return playing; // isPlaying -> playing으로 변경
}

void BuzzerManager::stop()
{
    playing = false; // isPlaying -> playing으로 변경
    isPaused = false;
    digitalWrite(buzzerPin, LOW);
}
