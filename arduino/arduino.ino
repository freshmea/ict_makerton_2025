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
    // 터치 상태 업데이트 (매 루프마다 호출 필요)
    robot.updateTouchStates();

    // // 터치 시작 감지
    // if (robot.isTouchPressed(1))
    // {
    //     robot.moveServoSmooth(1, 180);
    //     robot.fillColor(255, 0, 0);
    //     robot.beep(200);
    // }

    // if (robot.isTouchPressed(2))
    // {
    //     robot.moveServoSmooth(2, 0);
    //     robot.fillColor(0, 255, 0);
    //     robot.beep(200);
    // }

    // if (robot.isTouchPressed(3))
    // {
    //     robot.moveBothServos(90, 90);
    //     robot.clearPixels();
    //     robot.beepPattern(2, 100, 100);
    // }

    // // 롱프레스 감지 (2초 이상)
    // if (robot.isTouchLongPress(1, 2000))
    // {
    //     robot.lcdPrint(0, 0, "Long Press 1!");
    //     robot.fillColor(255, 255, 0);
    // }

    // if (robot.isTouchLongPress(2, 2000))
    // {
    //     robot.lcdPrint(0, 0, "Long Press 2!");
    //     robot.fillColor(255, 0, 255);
    // }

    // if (robot.isTouchLongPress(3, 2000))
    // {
    //     robot.lcdPrint(0, 0, "Long Press 3!");
    //     robot.fillColor(0, 255, 255);
    // }

    // // 터치 해제 감지
    // if (robot.isTouchReleased(1) || robot.isTouchReleased(2) || robot.isTouchReleased(3))
    // {
    //     robot.clearPixels();
    // }

    // 상태 업데이트 (터치 시간 포함)
    // robot.updateStatusDisplay();

    robot.updateMessage();
}
