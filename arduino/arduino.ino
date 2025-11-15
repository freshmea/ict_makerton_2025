// #include <Arduino.h>

// 기초 프로그램
void setup()
{
    // 시리얼 통신 시작
    Serial.begin(9600);

    // 내장 LED 핀을 출력으로 설정 (연결 확인용)
    pinMode(LED_BUILTIN, OUTPUT);

    // 시리얼 통신이 준비될 때까지 대기
    while (!Serial)
    {
        ; // 시리얼 포트 연결 대기
    }

    // 시작 메시지
    Serial.println("Arduino is ready!");
}

void loop()
{
    // "Hello, ICT Makerton 2025!" 메시지를 시리얼 모니터에 출력
    Serial.println("Hello, ICT Makerton 2025!");

    // 내장 LED 깜빡임 (보드 작동 확인용)
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    // 0.5초 대기
    delay(500);
}
