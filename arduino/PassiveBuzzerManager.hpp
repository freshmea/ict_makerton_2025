#ifndef PASSIVEBUZZERMANAGER_HPP
#define PASSIVEBUZZERMANAGER_HPP

#include <Arduino.h>

struct MelodyNote
{
    int frequency;
    int duration;
};

class PassiveBuzzerManager
{
private:
    int buzzerPin;
    bool isPlaying;
    unsigned long playStartTime;
    unsigned long currentNoteStartTime;
    int currentNoteIndex;

    // 멜로디 큐 (최대 32개 노트)
    static const int MAX_NOTES = 32;
    MelodyNote melodyQueue[MAX_NOTES];
    int queueStart;
    int queueEnd;
    int queueSize;

    // 현재 재생 중인 노트 정보
    bool noteActive;

public:
    PassiveBuzzerManager(int pin = 2);
    void init();
    void update(unsigned long currentMillis);

    // 단일 노트 추가
    void addNote(int frequency, int duration);

    // 멜로디 배열 추가
    void addMelody(MelodyNote *melody, int noteCount);

    // 재생 제어
    void play();
    void stop();
    void clear();

    // 상태 확인
    bool getIsPlaying();
    int getQueueSize();
    bool isQueueFull();

    // 미리 정의된 멜로디들
    void playBeep(int frequency = 1000, int duration = 200);
    void playSuccess();
    void playError();
    void playStartup();

    // 클래식 & 전통 멜로디들
    void playHappyBirthday();
    void playTwinkleTwinkleLittleStar();
    void playMaryHadALittleLamb();
    void playFurElise();
    void playOdeToJoy();
    void playCannonInD();
    void playAmazingGrace();
    void playGreensleeves();
    void playAuLaitClair();
    void playBrahmsLullaby();
};

#endif
