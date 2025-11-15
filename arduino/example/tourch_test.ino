// ===== 터치 센서 설정 =====
#define TOUCH_PIN 8

// ===== 터치 센서 상태 변수 =====
bool lastTouchState = false;
bool currentTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50; // 디바운싱 딜레이 (50ms)

void setup()
{
    // 시리얼 통신 시작
    Serial.begin(9600);

    // 터치 센서 핀을 입력으로 설정 (내부 풀업 저항 사용)
    pinMode(TOUCH_PIN, INPUT_PULLUP);

    // 내장 LED 핀을 출력으로 설정 (터치 상태 표시용)
    pinMode(LED_BUILTIN, OUTPUT);

    // 시작 메시지
    Serial.println("Touch Sensor Test Ready!");
    Serial.println("Touch sensor connected to pin 8");
    Serial.println("Waiting for touch...");
}

void loop()
{
    // 터치 센서 상태 읽기 (LOW = 터치됨, HIGH = 터치안됨)
    bool reading = !digitalRead(TOUCH_PIN); // 반전하여 true = 터치됨

    // 디바운싱 처리
    if (reading != lastTouchState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
    {
        // 상태가 실제로 변경되었을 때만 처리
        if (reading != currentTouchState)
        {
            currentTouchState = reading;

            if (currentTouchState)
            {
                // 터치 감지됨
                Serial.println("TOUCH DETECTED!");
                Serial.print("Timestamp: ");
                Serial.println(millis());
                digitalWrite(LED_BUILTIN, HIGH);
            }
            else
            {
                // 터치 해제됨
                Serial.println("Touch released");
                digitalWrite(LED_BUILTIN, LOW);
            }
        }
    }

    lastTouchState = reading;

    // 작은 딜레이 (CPU 부하 감소)
    delay(10);
}
