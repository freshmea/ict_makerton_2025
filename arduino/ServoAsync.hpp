#ifndef SERVOASYNC_H
#define SERVOASYNC_H

#include <Arduino.h>
#include <Servo.h>

// 서보 움직임 큐 구조체
struct ServoCommand
{
    unsigned long timestamp; // millis() 타임스탬프
    int targetAngle;         // 목표 각도
};

class ServoAsync
{
private:
    Servo servo;
    int servoPin;
    int currentAngle;

    // 큐 버퍼 시스템 (10Hz로 30초 = 300개)
    static const int QUEUE_SIZE = 300;
    static const int UPDATE_INTERVAL = 100; // 100ms = 10Hz

    ServoCommand commandQueue[QUEUE_SIZE];
    int queueHead;  // 큐의 시작 인덱스
    int queueTail;  // 큐의 끝 인덱스
    int queueCount; // 현재 큐에 있는 명령 개수

    unsigned long lastUpdateTime;     // 마지막 업데이트 시간
    unsigned long lastQueueCleanTime; // 마지막 큐 정리 시간

public:
    ServoAsync(int pin);

    void init();
    void update(unsigned long currentMillis);

    void moveToAngle(int angle);
    void moveSmooth(int targetAngle, int durationMs, unsigned long currentMillis = 0);
    void addToQueue(unsigned long timestamp, int angle);
    void cleanOldCommands(unsigned long currentMillis);

    int getCurrentAngle();
    bool isQueueEmpty();
    int getQueueSize();
};

#endif
