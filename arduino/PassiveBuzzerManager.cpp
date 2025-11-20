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
}

void PassiveBuzzerManager::update()
{
    if (!isPlaying || queueSize == 0)
        return;

    unsigned long currentTime = millis();

    // 첫 번째 노트 시작
    if (currentNoteIndex == 0 && !noteActive)
    {
        playStartTime = currentTime;
        currentNoteStartTime = currentTime;
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

        if (currentTime - currentNoteStartTime >= currentNote.duration)
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
                currentNoteStartTime = currentTime;
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
    addNote(523, 200); // C5
    addNote(659, 200); // E5
    addNote(784, 300); // G5
}

void PassiveBuzzerManager::playError()
{
    // 에러 멜로디: 낮은 음 2회
    addNote(200, 300);
    addNote(0, 100); // 휴지표
    addNote(200, 300);
}

void PassiveBuzzerManager::playStartup()
{
    // 시작 멜로디: C-D-E-F-G
    addNote(523, 150); // C5
    addNote(587, 150); // D5
    addNote(659, 150); // E5
    addNote(698, 150); // F5
    addNote(784, 300); // G5
}
