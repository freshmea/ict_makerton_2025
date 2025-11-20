#include "PassiveBuzzerManager.hpp"

PassiveBuzzerManager::PassiveBuzzerManager(int pin)
{
    buzzerPin = pin;
    isPlaying = false;
    playStartTime = 0;
    currentNoteStartTime = 0;
    currentNoteIndex = 0;
    queueStart = 0;
    queueEnd = 0;
    queueSize = 0;
    noteActive = false;
}

void PassiveBuzzerManager::init()
{
    pinMode(buzzerPin, OUTPUT);
    noTone(buzzerPin);
    playStartup();
}

void PassiveBuzzerManager::update(unsigned long currentMillis)
{
    if (!isPlaying || queueSize == 0)
        return;

    // 첫 번째 노트 시작
    if (currentNoteIndex == 0 && !noteActive)
    {
        playStartTime = currentMillis;
        currentNoteStartTime = currentMillis;
        noteActive = true;

        MelodyNote &note = melodyQueue[queueStart];
        if (note.frequency > 0)
        {
            tone(buzzerPin, note.frequency);
        }
        else
        {
            noTone(buzzerPin); // 휴지표
        }
    }

    // 현재 노트 재생 시간 체크
    if (noteActive)
    {
        MelodyNote &currentNote = melodyQueue[(queueStart + currentNoteIndex) % MAX_NOTES];

        if (currentMillis - currentNoteStartTime >= currentNote.duration)
        {
            // 현재 노트 종료
            noTone(buzzerPin);
            noteActive = false;
            currentNoteIndex++;

            // 다음 노트로 진행
            if (currentNoteIndex >= queueSize)
            {
                // 모든 노트 재생 완료
                stop();
            }
            else
            {
                // 다음 노트 시작 준비
                currentNoteStartTime = currentMillis;
                noteActive = true;

                MelodyNote &nextNote = melodyQueue[(queueStart + currentNoteIndex) % MAX_NOTES];
                if (nextNote.frequency > 0)
                {
                    tone(buzzerPin, nextNote.frequency);
                }
                else
                {
                    noTone(buzzerPin); // 휴지표
                }
            }
        }
    }
}

void PassiveBuzzerManager::addNote(int frequency, int duration)
{
    if (isQueueFull())
        return; // 큐가 가득 참

    melodyQueue[queueEnd].frequency = frequency;
    melodyQueue[queueEnd].duration = duration;

    queueEnd = (queueEnd + 1) % MAX_NOTES;
    queueSize++;

    // 재생 중이 아니면 자동으로 재생 시작
    if (!isPlaying)
    {
        play();
    }
}

void PassiveBuzzerManager::addMelody(MelodyNote *melody, int noteCount)
{
    stop(); // 현재 재생 중지
    for (int i = 0; i < noteCount && !isQueueFull(); i++)
    {
        melodyQueue[queueEnd].frequency = melody[i].frequency;
        melodyQueue[queueEnd].duration = melody[i].duration;

        queueEnd = (queueEnd + 1) % MAX_NOTES;
        queueSize++;
    }

    // 재생 중이 아니면 자동으로 재생 시작
    if (!isPlaying)
    {
        play();
    }
}

void PassiveBuzzerManager::play()
{
    if (queueSize > 0 && !isPlaying)
    {
        isPlaying = true;
        currentNoteIndex = 0;
        noteActive = false;
    }
}

void PassiveBuzzerManager::stop()
{
    isPlaying = false;
    noteActive = false;
    currentNoteIndex = 0;
    noTone(buzzerPin);

    // 큐 초기화
    clear();
}

void PassiveBuzzerManager::clear()
{
    queueStart = 0;
    queueEnd = 0;
    queueSize = 0;
}

bool PassiveBuzzerManager::getIsPlaying()
{
    return isPlaying;
}

int PassiveBuzzerManager::getQueueSize()
{
    return queueSize;
}

bool PassiveBuzzerManager::isQueueFull()
{
    return queueSize >= MAX_NOTES;
}

void PassiveBuzzerManager::playBeep(int frequency, int duration)
{
    addNote(frequency, duration);
}

void PassiveBuzzerManager::playSuccess()
{
    // 성공 멜로디: C-E-G
    MelodyNote successMelody[] = {
        {523, 200}, {0, 50}, // C5 + 휴지표
        {659, 200},
        {0, 50},   // E5 + 휴지표
        {784, 300} // G5
    };
    addMelody(successMelody, sizeof(successMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playError()
{
    // 에러 멜로디: 낮은 음 2회
    MelodyNote errorMelody[] = {
        {200, 300}, {0, 100}, // 낮은 음 + 휴지표
        {200, 300}            // 낮은 음
    };
    addMelody(errorMelody, sizeof(errorMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playStartup()
{
    // 시작 멜로디: C-D-E-F-G
    MelodyNote startupMelody[] = {
        {523, 150}, {0, 30}, // C5 + 휴지표
        {587, 150},
        {0, 30}, // D5 + 휴지표
        {659, 150},
        {0, 30}, // E5 + 휴지표
        {698, 150},
        {0, 30},   // F5 + 휴지표
        {784, 300} // G5
    };
    addMelody(startupMelody, sizeof(startupMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playHappyBirthday()
{
    // Happy Birthday (전통 민요)
    MelodyNote happyBirthdayMelody[] = {
        {523, 250}, {0, 50}, // C + 휴지표
        {523, 250},
        {0, 50}, // C + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {698, 500},
        {0, 100}, // F + 휴지표
        {659, 1000},
        {0, 200}, // E + 휴지표
        {523, 250},
        {0, 50}, // C + 휴지표
        {523, 250},
        {0, 50}, // C + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {784, 500},
        {0, 100},   // G + 휴지표
        {698, 1000} // F
    };
    addMelody(happyBirthdayMelody, sizeof(happyBirthdayMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playTwinkleTwinkleLittleStar()
{
    // Twinkle Twinkle Little Star (전통 민요)
    MelodyNote twinkleMelody[] = {
        {523, 500}, {0, 100}, // C + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {784, 500},
        {0, 100}, // G + 휴지표
        {784, 500},
        {0, 100}, // G + 휴지표
        {880, 500},
        {0, 100}, // A + 휴지표
        {880, 500},
        {0, 100}, // A + 휴지표
        {784, 1000},
        {0, 200}, // G + 휴지표
        {698, 500},
        {0, 100}, // F + 휴지표
        {698, 500},
        {0, 100}, // F + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {587, 500},
        {0, 100},   // D + 휴지표
        {523, 1000} // C
    };
    addMelody(twinkleMelody, sizeof(twinkleMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playMaryHadALittleLamb()
{
    // Mary Had a Little Lamb (전통 민요)
    MelodyNote maryMelody[] = {
        {659, 500}, {0, 100}, // E + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {659, 1000},
        {0, 200}, // E + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {587, 1000},
        {0, 200}, // D + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {784, 500},
        {0, 100},   // G + 휴지표
        {784, 1000} // G
    };
    addMelody(maryMelody, sizeof(maryMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playFurElise()
{
    // Für Elise - Beethoven (첫 부분, 퍼블릭 도메인)
    MelodyNote furEliseMelody[] = {
        {659, 300}, {0, 50}, // E + 휴지표
        {622, 300},
        {0, 50}, // D# + 휴지표
        {659, 300},
        {0, 50}, // E + 휴지표
        {622, 300},
        {0, 50}, // D# + 휴지표
        {659, 300},
        {0, 50}, // E + 휴지표
        {494, 300},
        {0, 50}, // B + 휴지표
        {587, 300},
        {0, 50}, // D + 휴지표
        {523, 300},
        {0, 50}, // C + 휴지표
        {440, 600},
        {0, 200}, // A + 휴지표
        {262, 300},
        {0, 50}, // C4 + 휴지표
        {330, 300},
        {0, 50}, // E4 + 휴지표
        {440, 300},
        {0, 50},   // A4 + 휴지표
        {494, 600} // B4
    };
    addMelody(furEliseMelody, sizeof(furEliseMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playOdeToJoy()
{
    // Ode to Joy - Beethoven (퍼블릭 도메인)
    MelodyNote odeToJoyMelody[] = {
        {659, 500}, {0, 100}, // E + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {698, 500},
        {0, 100}, // F + 휴지표
        {784, 500},
        {0, 100}, // G + 휴지표
        {784, 500},
        {0, 100}, // G + 휴지표
        {698, 500},
        {0, 100}, // F + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {659, 500},
        {0, 100}, // E + 휴지표
        {659, 750},
        {0, 150}, // E + 휴지표
        {587, 250},
        {0, 50},    // D + 휴지표
        {587, 1000} // D
    };
    addMelody(odeToJoyMelody, sizeof(odeToJoyMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playCannonInD()
{
    // Canon in D - Pachelbel (첫 부분, 퍼블릭 도메인)
    MelodyNote cannonMelody[] = {
        {587, 1000}, {0, 200}, // D + 휴지표
        {440, 500},
        {0, 100}, // A + 휴지표
        {494, 500},
        {0, 100}, // B + 휴지표
        {622, 500},
        {0, 100}, // F# + 휴지표
        {784, 500},
        {0, 100}, // G + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {784, 500},
        {0, 100}, // G + 휴지표
        {440, 1000},
        {0, 200}, // A + 휴지표
        {587, 1000},
        {0, 200}, // D + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {440, 500},
        {0, 100}, // A + 휴지표
        {494, 500},
        {0, 100},   // B + 휴지표
        {622, 1000} // F#
    };
    addMelody(cannonMelody, sizeof(cannonMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playAmazingGrace()
{
    // Amazing Grace (전통 찬송가, 퍼블릭 도메인)
    MelodyNote amazingGraceMelody[] = {
        {392, 750}, {0, 150}, // G + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {523, 250},
        {0, 50}, // C + 휴지표
        {440, 500},
        {0, 100}, // A + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {440, 750},
        {0, 150}, // A + 휴지표
        {349, 250},
        {0, 50}, // F + 휴지표
        {392, 1000},
        {0, 500}, // G + 휴지표
        {392, 750},
        {0, 150}, // G + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {523, 250},
        {0, 50}, // C + 휴지표
        {587, 500},
        {0, 100},   // D + 휴지표
        {523, 1500} // C
    };
    addMelody(amazingGraceMelody, sizeof(amazingGraceMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playGreensleeves()
{
    // Greensleeves (전통 영국 민요, 퍼블릭 도메인)
    MelodyNote greensleavesMelody[] = {
        {440, 500}, {0, 100}, // A + 휴지표
        {523, 750},
        {0, 150}, // C + 휴지표
        {587, 250},
        {0, 50}, // D + 휴지표
        {622, 500},
        {0, 100}, // E♭ + 휴지표
        {698, 250},
        {0, 50}, // F + 휴지표
        {622, 250},
        {0, 50}, // E♭ + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {494, 750},
        {0, 150}, // B♭ + 휴지표
        {392, 250},
        {0, 50}, // G + 휴지표
        {440, 500},
        {0, 100}, // A + 휴지표
        {466, 250},
        {0, 50}, // B♭ + 휴지표
        {440, 250},
        {0, 50}, // A + 휴지표
        {392, 500},
        {0, 100},   // G + 휴지표
        {349, 1000} // F
    };
    addMelody(greensleavesMelody, sizeof(greensleavesMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playAuLaitClair()
{
    // Au Clair de la Lune (프랑스 전통 민요, 퍼블릭 도메인)
    MelodyNote auLaitClairMelody[] = {
        {523, 500}, {0, 100}, // C + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {659, 1000},
        {0, 200}, // E + 휴지표
        {587, 1000},
        {0, 200}, // D + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {587, 500},
        {0, 100}, // D + 휴지표
        {523, 500},
        {0, 100}, // C + 휴지표
        {587, 500},
        {0, 100},   // D + 휴지표
        {523, 2000} // C
    };
    addMelody(auLaitClairMelody, sizeof(auLaitClairMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playBrahmsLullaby()
{
    // Brahms Lullaby (퍼블릭 도메인)
    MelodyNote brahmsLullabyMelody[] = {
        {392, 750}, {0, 150}, // G + 휴지표
        {392, 250},
        {0, 50}, // G + 휴지표
        {440, 500},
        {0, 100}, // A + 휴지표
        {392, 750},
        {0, 150}, // G + 휴지표
        {440, 250},
        {0, 50}, // A + 휴지표
        {392, 500},
        {0, 100}, // G + 휴지표
        {349, 500},
        {0, 100}, // F + 휴지표
        {349, 500},
        {0, 100}, // F + 휴지표
        {329, 1000},
        {0, 200}, // E + 휴지표
        {392, 750},
        {0, 150}, // G + 휴지표
        {392, 250},
        {0, 50}, // G + 휴지표
        {440, 500},
        {0, 100}, // A + 휴지표
        {392, 750},
        {0, 150}, // G + 휴지표
        {349, 250},
        {0, 50},    // F + 휴지표
        {294, 1500} // D
    };
    addMelody(brahmsLullabyMelody, sizeof(brahmsLullabyMelody) / sizeof(MelodyNote));
}

void PassiveBuzzerManager::playRandom()
{
    // 랜덤 멜로디 재생 (0~8 사이의 랜덤 숫자)
    int randomMelody = random(0, 9);

    switch (randomMelody)
    {
    case 0:
        playSuccess();
        break;
    case 1:
        playHappyBirthday();
        break;
    case 2:
        playTwinkleTwinkleLittleStar();
        break;
    case 3:
        playMaryHadALittleLamb();
        break;
    case 4:
        playFurElise();
        break;
    case 5:
        playOdeToJoy();
        break;
    case 6:
        playCannonInD();
        break;
    case 7:
        playAmazingGrace();
        break;
    case 8:
        playBrahmsLullaby();
        break;
    default:
        playBeep(1000, 200); // 기본 비프음
        break;
    }
}