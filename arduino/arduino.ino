#include "SoneeBot.hpp"

// SoneeBot 객체 생성 (기본 핀 설정 사용)
SoneeBot robot;

void setup()
{
    // SoneeBot 초기화 - 모든 장치 초기화 포함
    robot.init();

    // 초기화 완료 후 테스트 실행
    // robot.testAllDevices();
}

void loop()
{
    // robot update
    robot.update(millis());
}
