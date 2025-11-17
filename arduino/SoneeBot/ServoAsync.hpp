#ifndef SERVOASYNC_H
#define SERVOASYNC_H

#include <Arduino.h>
#include <Servo.h>

class ServoAsync
{
private:
    Servo servo;
    int servoPin;
    int currentAngle;

    // 메모리 최적화: 큐를 최소한으로 축소
    static const int QUEUE_SIZE = 200;
    static const int UPDATE_INTERVAL = 10;

    unsigned char angleBuffer[QUEUE_SIZE];
    unsigned long firstAngleMillis;
    int currentIndex;
    int currentRound;
    int bufferCount;

    unsigned long lastUpdateTime;

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
