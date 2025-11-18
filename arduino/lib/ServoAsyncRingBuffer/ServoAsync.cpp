#include "ServoAsync.hpp"

ServoAsync::ServoAsync(int pin)
{
    servoPin = pin;
    currentAngle = 90; // 기본 각도

    // 링버퍼 초기화
    firstAngleMillis = 0;
    currentIndex = 0;
    currentRound = 0;
    bufferCount = 0;
    lastUpdateTime = 0;

    // 배열 초기화
    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        angleBuffer[i] = 90; // 기본값으로 90도 설정
    }
}

void ServoAsync::init()
{
    servo.attach(servoPin);
    servo.write(currentAngle);
}

void ServoAsync::update(unsigned long currentMillis)
{
    // 100Hz 업데이트 (10ms 간격)
    if (currentMillis - lastUpdateTime >= UPDATE_INTERVAL)
    {
        if (!isQueueEmpty())
        {
            // 현재 시간을 기준으로 실행할 인덱스 계산
            unsigned long elapsedTime = currentMillis - firstAngleMillis;
            int targetIndex = (int)(elapsedTime / UPDATE_INTERVAL);

            // 링버퍼 인덱스 계산 (바퀴 수 고려)
            int bufferIndex = targetIndex % QUEUE_SIZE;
            int targetRound = targetIndex / QUEUE_SIZE;

            // 현재 바퀴와 타겟 바퀴가 같고, 버퍼에 데이터가 있는 경우에만 실행
            if (targetRound == currentRound && targetIndex < bufferCount)
            {
                int targetAngle = angleBuffer[bufferIndex];
                servo.write(targetAngle);
                currentAngle = targetAngle; // 현재 각도 업데이트 추가

                // 인덱스 업데이트
                currentIndex = bufferIndex;
            }

            // 바퀴가 넘어간 경우 정리
            if (targetRound > currentRound)
            {
                currentRound = targetRound;
                bufferCount = 0; // 새로운 바퀴 시작시 버퍼 리셋
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

    // 즉시 실행이므로 버퍼 클리어
    bufferCount = 0;
    firstAngleMillis = 0;
}

void ServoAsync::moveSmooth(int targetAngle, int durationMs, unsigned long currentMillis)
{
    targetAngle = constrain(targetAngle, 0, 180);

    if (currentMillis == 0)
    {
        currentMillis = millis();
    }

    int startAngle = currentAngle;
    int steps = abs(targetAngle - startAngle);

    if (steps == 0)
        return;

    // 각 UPDATE_INTERVAL(100ms)마다 실행할 중간 각도들을 버퍼에 추가
    int commandCount = (durationMs / UPDATE_INTERVAL) + 1;

    for (int i = 0; i < commandCount; i++)
    {
        unsigned long timestamp = currentMillis + (i * UPDATE_INTERVAL);
        float progress = (float)i / (float)commandCount;
        int angle = startAngle + (int)((targetAngle - startAngle) * progress);

        addToQueue(timestamp, angle);
    }
}

void ServoAsync::addToQueue(unsigned long timestamp, int angle)
{
    // 첫 번째 항목인 경우에만 시작 시간 설정
    if (bufferCount == 0)
    {
        firstAngleMillis = timestamp;
        currentRound = 0;
        currentIndex = 0;
    }

    // 타임스탬프를 인덱스로 변환
    unsigned long elapsedTime = timestamp - firstAngleMillis;
    int totalIndex = (int)(elapsedTime / UPDATE_INTERVAL);

    // 링버퍼 인덱스와 라운드 계산
    int bufferIndex = totalIndex % QUEUE_SIZE;
    int targetRound = totalIndex / QUEUE_SIZE;

    // 버퍼에 각도 저장
    angleBuffer[bufferIndex] = constrain(angle, 0, 180);

    // 버퍼 카운트 업데이트
    if (totalIndex >= bufferCount)
    {
        bufferCount = totalIndex + 1;
    }

    // 링버퍼가 한 바퀴 돌았을 때 처리
    if (bufferIndex == 0 && totalIndex > 0)
    {
        // 새로운 라운드 시작 - 이전 라운드 데이터는 덮어씌워짐
    }
}

void ServoAsync::cleanOldCommands(unsigned long currentMillis)
{
    // 현재 시간보다 1초 이상 오래된 명령들 무시 (링버퍼 특성상 자동으로 처리됨)
    if (firstAngleMillis > 0 && currentMillis - firstAngleMillis > 30000) // 30초 후 리셋
    {
        bufferCount = 0;
        firstAngleMillis = 0;
        currentRound = 0;
        currentIndex = 0;
    }
}

int ServoAsync::getCurrentAngle()
{
    return currentAngle;
}

bool ServoAsync::isQueueEmpty()
{
    return bufferCount == 0;
}

int ServoAsync::getQueueSize()
{
    if (bufferCount == 0 || firstAngleMillis == 0)
    {
        return 0; // 버퍼가 비어있으면 0 반환
    }

    unsigned long currentMillis = millis();

    // 현재 시간을 기준으로 실행된 인덱스 계산
    unsigned long elapsedTime = currentMillis - firstAngleMillis;
    int currentExecutedIndex = (int)(elapsedTime / UPDATE_INTERVAL);

    // 버퍼에 저장된 마지막 인덱스 (bufferCount - 1)
    int lastBufferedIndex = bufferCount - 1;

    // 앞으로 실행해야 할 버퍼 개수 계산
    int remainingCount = lastBufferedIndex - currentExecutedIndex;

    // 음수가 나오면 0 반환 (모든 명령이 이미 실행됨)
    return (remainingCount > 0) ? remainingCount : 0;
}
