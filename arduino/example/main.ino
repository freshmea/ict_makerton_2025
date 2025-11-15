#include <DHT.h>
#include <LiquidCrystal.h>

// 핀 정의
#define DHT_PIN 2
#define DHT_TYPE DHT22 // 또는 DHT11
#define BUZZER_PIN 13

// LCD 핀 연결 (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 6);

// DHT 센서 객체
DHT dht(DHT_PIN, DHT_TYPE);

void setup()
{
    // 시리얼 통신 시작
    Serial.begin(9600);

    // 내장 LED 핀을 출력으로 설정
    pinMode(LED_BUILTIN, OUTPUT);

    // 버저 핀 설정
    pinMode(BUZZER_PIN, OUTPUT);

    // DHT 센서 초기화
    dht.begin();

    // LCD 초기화 (16x2)
    lcd.begin(16, 2);
    lcd.clear();

    // 시작 메시지
    Serial.println("Arduino Multi-Sensor System Ready!");
    lcd.setCursor(0, 0);
    lcd.print("System Ready!");

    // 시작 부저음
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);

    delay(2000);
}

void loop()
{
    // DHT 센서에서 온습도 읽기
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // 센서 읽기 확인
    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("DHT 센서 읽기 실패!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sensor Error!");
        return;
    }

    // 시리얼 모니터에 출력
    Serial.print("온도: ");
    Serial.print(temperature);
    Serial.print("°C  습도: ");
    Serial.print(humidity);
    Serial.println("%");

    // LCD에 온습도 표시
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Humi: ");
    lcd.print(humidity, 1);
    lcd.print("%");

    // 온도가 30도 이상이면 경고음
    if (temperature > 30.0)
    {
        for (int i = 0; i < 3; i++)
        {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
            delay(100);
        }
    }

    // LED 깜빡임
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    // 2초 대기
    delay(1500);
}
