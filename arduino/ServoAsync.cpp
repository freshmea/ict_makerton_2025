#include "ServoAsync.hpp"

ServoAsync::ServoAsync(int pin)
{
    servoPin = pin;
    currentAngle = 90; // 기본 각도

    // 큐 버퍼 초기화
    queueHead = 0;
    queueTail = 0;
    queueCount = 0;
    lastUpdateTime = 0;
    lastQueueCleanTime = 0;
}

void ServoAsync::init()
{
    servo.attach(servoPin);
    servo.write(currentAngle);
}

void ServoAsync::update(unsigned long currentMillis)
{
    // 오래된 명령들 정리 (1초마다)
    if (currentMillis - lastQueueCleanTime >= 1000)
    {
        cleanOldCommands(currentMillis);
        lastQueueCleanTime = currentMillis;
    }

    // 10Hz 업데이트 (100ms 간격)
    if (currentMillis - lastUpdateTime >= UPDATE_INTERVAL)
    {
        if (!isQueueEmpty())
        {
            // 현재 시간에 실행할 명령 찾기
            ServoCommand *currentCommand = nullptr;

            // 큐에서 현재 시간 이후의 가장 가까운 명령 찾기
            for (int i = 0; i < queueCount; i++)
            {
                int index = (queueHead + i) % QUEUE_SIZE;
                if (commandQueue[index].timestamp <= currentMillis)
                {
                    currentCommand = &commandQueue[index];
                }
                else
                {
                    break; // 시간순으로 정렬되어 있으므로 더 이상 찾을 필요 없음
                }
            }

            // 실행할 명령이 있으면 서보 움직임
            if (currentCommand != nullptr)
            {
                // 현재 각도에서 목표 각도로 한 스텝씩 이동
                int step = 0;

                if (currentAngle < currentCommand->targetAngle)
                    step = 1;
                else if (currentAngle > currentCommand->targetAngle)
                    step = -1;

                // 서보 움직임 실행
                if (step != 0)
                {
                    currentAngle += step;
                    servo.write(currentAngle);

                    Serial.print("Servo Pin ");
                    Serial.print(servoPin);
                    Serial.print(" Update: ");
                    Serial.print(currentAngle);
                    Serial.print("° -> ");
                    Serial.print(currentCommand->targetAngle);
                    Serial.println("°");
                }
            }
        }

        lastUpdateTime = currentMillis;
    }
}

void ServoAsync::moveToAngle(int angle)
{
    angle = constrain(angle, 0, 180);
    servo.write(angle);
    currentAngle = angle;
}

void ServoAsync::moveSmooth(int targetAngle, int durationMs, unsigned long currentMillis)
{
    targetAngle = constrain(targetAngle, 0, 180);

    // currentMillis가 0이면 현재 시간을 가져옴 (호환성을 위해)
    if (currentMillis == 0)
    {
        currentMillis = millis();
    }

    int startAngle = currentAngle;
    int steps = abs(targetAngle - startAngle);

    if (steps == 0)
        return; // 움직일 필요 없음

    // 각 UPDATE_INTERVAL(100ms)마다 실행할 중간 각도들을 큐에 추가
    int commandCount = (durationMs / UPDATE_INTERVAL) + 1;

    for (int i = 0; i <= commandCount; i++)
    {
        unsigned long timestamp = currentMillis + (i * UPDATE_INTERVAL);
        float progress = (float)i / (float)commandCount;

        int angle = startAngle + (int)((targetAngle - startAngle) * progress);

        addToQueue(timestamp, angle);
    }

    Serial.print("Smooth motion queued - Pin ");
    Serial.print(servoPin);
    Serial.print(" from ");
    Serial.print(startAngle);
    Serial.print("° to ");
    Serial.print(targetAngle);
    Serial.print("° over ");
    Serial.print(durationMs);
    Serial.print("ms (");
    Serial.print(commandCount + 1);
    Serial.println(" commands)");
}

void ServoAsync::addToQueue(unsigned long timestamp, int angle)
{
    // 큐가 가득 찬 경우 오래된 명령 제거
    if (queueCount >= QUEUE_SIZE)
    {
        queueHead = (queueHead + 1) % QUEUE_SIZE;
        queueCount--;
        Serial.print("Warning: Servo queue full for pin ");
        Serial.print(servoPin);
        Serial.println(", removing oldest command");
    }

    // 새 명령 추가
    commandQueue[queueTail].timestamp = timestamp;
    commandQueue[queueTail].targetAngle = constrain(angle, 0, 180);

    queueTail = (queueTail + 1) % QUEUE_SIZE;
    queueCount++;
}

void ServoAsync::cleanOldCommands(unsigned long currentMillis)
{
    // 현재 시간보다 1초 이상 오래된 명령들 제거
    while (queueCount > 0)
    {
        int headIndex = queueHead;
        if (commandQueue[headIndex].timestamp < currentMillis - 1000)
        {
            queueHead = (queueHead + 1) % QUEUE_SIZE;
            queueCount--;
        }
        else
        {
            break; // 시간순으로 정렬되어 있으므로 더 이상 제거할 필요 없음
        }
    }
}

int ServoAsync::getCurrentAngle()
{
    return currentAngle;
}

bool ServoAsync::isQueueEmpty()
{
    return queueCount == 0;
}

int ServoAsync::getQueueSize()
{
    return queueCount;
}
