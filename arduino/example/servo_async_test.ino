#include "ServoAsync.hpp"

// ServoAsync 객체 생성 (핀 번호는 실제 연결된 핀에 맞게 수정)
ServoAsync servo1(10); // 10번 핀에 연결된 서보
ServoAsync servo2(11); // 11번 핀에 연결된 서보 (필요시)

void setup()
{
    Serial.begin(9600);
    Serial.println("ServoAsync moveSmooth 테스트 시작");

    // 서보 초기화
    servo1.init();
    servo2.init();

    // 초기 위치 설정 (90도)
    servo1.moveToAngle(90);
    servo2.moveToAngle(90);

    delay(1000);
}

void loop()
{
    static int cnt = 0;
    unsigned long currentTime = millis();

    // 서보 상태 업데이트 (매 루프마다 호출 필요)
    servo1.update(currentTime);
    servo2.update(currentTime);

    // moveSmooth 테스트: 다양한 속도로 움직임
    static unsigned long lastTime = 0;
    static int testPhase = 0;

    if (currentTime - lastTime > 5000) // 5초마다 테스트 변경
    {
        switch (testPhase % 4)
        {
        case 0: // 빠른 움직임 (1초)
            // Serial.println("=== 빠른 움직임 테스트 (1초) ===");
            servo1.moveSmooth(0, 1000, currentTime);
            servo2.moveSmooth(0, 1000, currentTime);
            break;

        case 1: // 중간 속도 (2초)
            // Serial.println("=== 중간 속도 테스트 (2초) ===");
            servo1.moveSmooth(140, 2000, currentTime);
            servo2.moveSmooth(70, 2000, currentTime);
            break;

        case 2: // 느린 움직임 (3초)
            // Serial.println("=== 느린 움직임 테스트 (3초) ===");
            servo1.moveSmooth(90, 3000, currentTime);
            servo2.moveSmooth(90, 3000, currentTime);
            break;

        case 3: // 매우 느린 움직임 (4초)
            // Serial.println("=== 매우 느린 움직임 테스트 (4초) ===");
            servo1.moveSmooth(0, 4000, currentTime);
            servo2.moveSmooth(135, 4000, currentTime);
            break;
        }

        Serial.print("테스트 단계: ");
        Serial.print(testPhase % 4 + 1);
        Serial.print("/4, 큐 크기1: ");
        Serial.print(servo1.getQueueSize());
        Serial.print(", 큐 크기2: ");
        Serial.println(servo2.getQueueSize());

        testPhase++;
        lastTime = currentTime;
    }
}
