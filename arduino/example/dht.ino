#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// ===== 핀 및 센서 설정 =====
#define DHTPIN 2      // 온습도 센서 데이터 핀 (디지털 2번)
#define DHTTYPE DHT11 // DHT11 사용 시. DHT22 사용 시 DHT22 로 변경
#define BUZZER_PIN 12 // 부저 핀 (디지털 12번)

// LCD I2C 주소(보통 0x27 또는 0x3F 중 하나)
// 16x2 LCD 사용 가정
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// ===== 불쾌지수 계산 함수 =====
// (기상청에서 많이 사용하는 식)
// DI = 9/5*T - 0.55*(1-RH/100)*(9/5*T - 26) + 32
// T: 섭씨온도(℃), RH: 상대습도(%)
float calcDiscomfortIndex(float T, float RH)
{
    float di = (9.0 / 5.0) * T - 0.55 * (1.0 - RH / 100.0) * ((9.0 / 5.0) * T - 26.0) + 32.0;
    return di;
}

// ===== 불쾌지수 수준에 따른 문자 반환 =====
const char *getLevelLabel(float di)
{
    // 기준 예시
    //  < 68      : 쾌적
    //  68 ~ 75   : 약간 불쾌
    //  75 ~ 80   : 상당히 불쾌
    //  >= 80     : 매우 불쾌
    if (di < 68.0)
    {
        return "GOOD ";
    }
    else if (di < 75.0)
    {
        return "MILD ";
    }
    else if (di < 80.0)
    {
        return "HIGH ";
    }
    else
    {
        return "VHIG "; // Very High
    }
}

void setup()
{
    // 시리얼 모니터(디버깅용)
    Serial.begin(9600);

    // DHT 센서, 부저, LCD 초기화
    dht.begin();
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    lcd.begin();     // init() 대신 begin() 사용
    lcd.backlight(); // 백라이트 ON

    lcd.setCursor(0, 0);
    lcd.print("Smart DI System");
    lcd.setCursor(0, 1);
    lcd.print("Init...Please");
    delay(2000);
    lcd.clear();
}

void loop()
{
    // 1. 온·습도 측정
    float h = dht.readHumidity();
    float t = dht.readTemperature(); // 섭씨 온도

    // 2. 센서값 유효성 체크
    if (isnan(h) || isnan(t))
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sensor Error");
        lcd.setCursor(0, 1);
        lcd.print("Check DHT");
        Serial.println("Failed to read from DHT sensor!");
        delay(2000);
        return;
    }

    // 3. 불쾌지수 계산
    float di = calcDiscomfortIndex(t, h);
    const char *level = getLevelLabel(di);

    // 4. LCD 표시
    // 1행: 온도, 습도
    // 예: T:25.3C H:60%
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(t, 1);
    lcd.print("C ");

    lcd.print("H:");
    lcd.print(h, 0);
    lcd.print("%");

    // 2행: 불쾌지수와 수준
    // 예: DI:75.2 HIGH
    lcd.setCursor(0, 1);
    lcd.print("DI:");
    lcd.print(di, 1);
    lcd.print(" ");
    lcd.print(level);

    // 5. 부저 알림
    // 기준 예시: 불쾌지수 75 이상일 때 알림
    if (di >= 75.0)
    {
        // 짧은 비프음을 두 번 울림
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(200);
            digitalWrite(BUZZER_PIN, LOW);
            delay(200);
        }
    }
    else
    {
        // 쾌적~보통일 때는 부저 OFF
        digitalWrite(BUZZER_PIN, LOW);
    }

    // 6. 디버깅 출력
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" C, Humidity: ");
    Serial.print(h);
    Serial.print(" %, DI: ");
    Serial.println(di);

    // 7. 측정 주기(예: 2초)
    delay(2000);
}
