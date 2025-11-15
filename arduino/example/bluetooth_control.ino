#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

// ===== 사용자 핀 설정 =====
#define LED_PIN 3     // 네오픽셀 데이터 DIN = D3
#define LED_COUNT 16  // LED 개수에 맞게 조정
#define BRIGHTNESS 64 // 0~255 (과전류 방지용으로 낮게 시작)

#define BT_RX_PIN 7 // 아두이노가 '수신'하는 핀 (BT의 TXD가 연결됨)
#define BT_TX_PIN 8 // 아두이노가 '송신'하는 핀 (BT의 RXD가 연결됨)

// ===== 객체 생성 =====
SoftwareSerial BT(BT_RX_PIN, BT_TX_PIN);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ===== 디버깅용 타이밍 변수 =====
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 1000; // 1초

// ===== 유틸 =====
void fillColor(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t c = strip.Color(r, g, b);
    for (int i = 0; i < LED_COUNT; i++)
        strip.setPixelColor(i, c);
    strip.show();
}

void handleCommand(char cmd)
{
    // 소문자도 허용
    if (cmd >= 'a' && cmd <= 'z')
        cmd = cmd - 'a' + 'A';

    switch (cmd)
    {
    case 'R': // 빨강
        fillColor(255, 0, 0);
        BT.println(F("OK:R"));
        break;
    case 'G': // 초록
        fillColor(0, 255, 0);
        BT.println(F("OK:G"));
        break;
    case 'B': // 파랑
        fillColor(0, 0, 255);
        BT.println(F("OK:B"));
        break;
    case 'W': // 하양 (RGB 스트립은 R=G=B)
        fillColor(255, 255, 255);
        BT.println(F("OK:W"));
        break;
    case 'O': // 끄기(Off)
        fillColor(0, 0, 0);
        BT.println(F("OK:OFF"));
        break;
    default:
        // 개행/공백 등 무시
        break;
    }
}

void setup()
{
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.show(); // 전부 끄기

    Serial.begin(115200); // PC 디버깅용
    BT.begin(9600);       // HC-05/06 기본 속도

    Serial.println(F("BT Mood Light Ready: send R,G,B,W,O"));
    BT.println(F("READY:R,G,B,W,O"));
}

void loop()
{
    // 블루투스 수신 처리
    if (BT.available())
    {
        char c = BT.read();
        handleCommand(c);
    }

    // (옵션) USB 시리얼에서도 동일 명령 지원
    if (Serial.available())
    {
        char c = Serial.read();
        handleCommand(c);
    }
}
