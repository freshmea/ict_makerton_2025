#include <Servo.h>

// ===== 서보 모터 설정 =====
Servo servo10;
Servo servo11;

// ===== 서보 움직임 제어 변수 =====
struct ServoControl
{
    int currentAngle;
    int targetAngle;
    int startAngle;
    unsigned long startTime;
    unsigned long duration;
    bool isMoving;
};

ServoControl servo10_ctrl = {90, 90, 90, 0, 0, false};
ServoControl servo11_ctrl = {90, 90, 90, 0, 0, false};

unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 10; // 10ms 간격으로 업데이트

// ===== 테스트 패턴 변수 =====
unsigned long lastPatternTime = 0;
const unsigned long PATTERN_INTERVAL = 2000; // 2초마다 새로운 움직임
int patternStep = 0;

void setup()
{
    Serial.begin(9600);

    // 서보 모터 연결 및 초기화
    servo10.attach(10);
    servo11.attach(11);
    servo10.write(90);
    servo11.write(90);

    Serial.println("Dual Servo Controller Ready!");
    Serial.println("Two servos will move in opposite directions");
    Serial.println("Commands: move [time_ms] [angle10] [angle11]");
    Serial.println("Example: move 1000 180 0");

    delay(1000);
}

void loop()
{
    unsigned long currentTime = millis();

    // 10ms마다 서보 위치 업데이트
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL)
    {
        lastUpdateTime = currentTime;
        updateServos(currentTime);
    }

    // 테스트 패턴 실행 (2초마다)
    if (currentTime - lastPatternTime >= PATTERN_INTERVAL)
    {
        lastPatternTime = currentTime;
        executeTestPattern();
    }

    // 시리얼 명령 처리
    if (Serial.available())
    {
        processSerialCommand();
    }
}

// 서보 위치 업데이트 함수
void updateServos(unsigned long currentTime)
{
    updateSingleServo(&servo10_ctrl, servo10, currentTime, "Servo10");
    updateSingleServo(&servo11_ctrl, servo11, currentTime, "Servo11");
}

// 개별 서보 업데이트
void updateSingleServo(ServoControl *ctrl, Servo &servo, unsigned long currentTime, const char *name)
{
    if (!ctrl->isMoving)
        return;

    unsigned long elapsed = currentTime - ctrl->startTime;

    if (elapsed >= ctrl->duration)
    {
        // 움직임 완료
        ctrl->currentAngle = ctrl->targetAngle;
        ctrl->isMoving = false;
        servo.write(ctrl->currentAngle);

        Serial.print(name);
        Serial.print(" reached target: ");
        Serial.println(ctrl->currentAngle);
    }
    else
    {
        // 중간 각도 계산 (선형 보간)
        float progress = (float)elapsed / ctrl->duration;
        int newAngle = ctrl->startAngle + (ctrl->targetAngle - ctrl->startAngle) * progress;

        if (newAngle != ctrl->currentAngle)
        {
            ctrl->currentAngle = newAngle;
            servo.write(ctrl->currentAngle);
        }
    }
}

// 서보 움직임 시작 함수
void moveServo(ServoControl *ctrl, int targetAngle, unsigned long duration, unsigned long startTime)
{
    ctrl->startAngle = ctrl->currentAngle;
    ctrl->targetAngle = constrain(targetAngle, 0, 180);
    ctrl->startTime = startTime;
    ctrl->duration = duration;
    ctrl->isMoving = true;
}

// 두 서보 동시 움직임
void moveBothServos(int angle10, int angle11, unsigned long duration)
{
    unsigned long currentTime = millis();

    moveServo(&servo10_ctrl, angle10, duration, currentTime);
    moveServo(&servo11_ctrl, angle11, duration, currentTime);

    Serial.print("Moving servos - S10:");
    Serial.print(angle10);
    Serial.print("° S11:");
    Serial.print(angle11);
    Serial.print("° Duration:");
    Serial.print(duration);
    Serial.println("ms");
}

// 테스트 패턴 실행
void executeTestPattern()
{
    switch (patternStep)
    {
    case 0:
        // 반대 방향으로 움직임: S10->180°, S11->0°
        moveBothServos(180, 0, 1000);
        Serial.println("Pattern: Opposite directions (180, 0)");
        break;

    case 1:
        // 중앙으로 복귀: S10->90°, S11->90°
        moveBothServos(90, 90, 800);
        Serial.println("Pattern: Return to center (90, 90)");
        break;

    case 2:
        // 다시 반대 방향: S10->0°, S11->180°
        moveBothServos(0, 180, 1000);
        Serial.println("Pattern: Opposite directions (0, 180)");
        break;

    case 3:
        // 중앙으로 복귀
        moveBothServos(90, 90, 800);
        Serial.println("Pattern: Return to center (90, 90)");
        break;

    case 4:
        // 동시에 한쪽으로
        moveBothServos(45, 135, 600);
        Serial.println("Pattern: Synchronized movement (45, 135)");
        break;

    case 5:
        // 동시에 반대쪽으로
        moveBothServos(135, 45, 600);
        Serial.println("Pattern: Synchronized movement (135, 45)");
        break;
    }

    patternStep = (patternStep + 1) % 6;
}

// 시리얼 명령 처리
void processSerialCommand()
{
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("move "))
    {
        // "move 1000 180 0" 형식 파싱
        int firstSpace = command.indexOf(' ', 5);
        int secondSpace = command.indexOf(' ', firstSpace + 1);

        if (firstSpace > 0 && secondSpace > 0)
        {
            unsigned long duration = command.substring(5, firstSpace).toInt();
            int angle10 = command.substring(firstSpace + 1, secondSpace).toInt();
            int angle11 = command.substring(secondSpace + 1).toInt();

            if (duration > 0 && duration <= 10000)
            {
                moveBothServos(angle10, angle11, duration);
            }
            else
            {
                Serial.println("Error: Duration must be 1-10000ms");
            }
        }
        else
        {
            Serial.println("Error: Use format 'move [time_ms] [angle10] [angle11]'");
        }
    }
    else if (command == "status")
    {
        Serial.print("Current positions - S10:");
        Serial.print(servo10_ctrl.currentAngle);
        Serial.print("° S11:");
        Serial.print(servo11_ctrl.currentAngle);
        Serial.print("° Moving: S10=");
        Serial.print(servo10_ctrl.isMoving ? "YES" : "NO");
        Serial.print(" S11=");
        Serial.println(servo11_ctrl.isMoving ? "YES" : "NO");
    }
    else if (command == "stop")
    {
        servo10_ctrl.isMoving = false;
        servo11_ctrl.isMoving = false;
        Serial.println("All servo movements stopped");
    }
    else if (command == "help")
    {
        Serial.println("Commands:");
        Serial.println("  move [time_ms] [angle10] [angle11] - Move both servos");
        Serial.println("  status - Show current positions");
        Serial.println("  stop - Stop all movements");
        Serial.println("  help - Show this help");
    }
}
