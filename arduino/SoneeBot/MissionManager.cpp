#include "MissionManager.hpp"

MissionManager::MissionManager()
{
    missionCount = 0;
    missionCompleted = false;
    missionCompleteTime = 0;
    lastMissionCount = 0;
    showingGoodJob = false;
    goodJobStartTime = 0;
}

void MissionManager::update(int touch1LastBeepCount, int touch2LastBeepCount, int touch1BeepCount, int touch2BeepCount, bool touch1State, bool touch2State)
{
    // Good Job 메시지 타이머 체크
    if (showingGoodJob)
    {
        unsigned long elapsed = millis() - goodJobStartTime;
        if (elapsed >= 1000)
        {
            showingGoodJob = false;
        }
    }

    // 현재 미션 카운트 계산
    int currentCount = calculateCurrentMissionCount(touch1LastBeepCount, touch2LastBeepCount, touch1BeepCount, touch2BeepCount, touch1State, touch2State);

    // 미션 완료 조건 체크
    if (!missionCompleted && lastMissionCount >= 1 && currentCount == 0)
    {
        completeMission();
    }

    missionCount = currentCount;

    // 미션 완료 효과 타이머 체크
    if (missionCompleted)
    {
        unsigned long elapsed = millis() - missionCompleteTime;
        if (elapsed >= 3000)
        {
            resetMissionComplete();
        }
    }

    lastMissionCount = currentCount;
}

int MissionManager::calculateCurrentMissionCount(int touch1LastBeepCount, int touch2LastBeepCount, int touch1BeepCount, int touch2BeepCount, bool touch1State, bool touch2State)
{
    int finalMissionCount = touch2LastBeepCount - touch1LastBeepCount;
    if (finalMissionCount < 0)
        finalMissionCount = 0;

    int displayCount;
    if (touch2State)
    {
        int currentIncrease = touch2BeepCount;
        displayCount = finalMissionCount + currentIncrease;
    }
    else if (touch1State)
    {
        int currentDecrease = touch1BeepCount;
        displayCount = finalMissionCount - currentDecrease;
        if (displayCount < 0)
            displayCount = 0;
    }
    else
    {
        displayCount = finalMissionCount;
    }

    return displayCount;
}

bool MissionManager::isMissionCompleted()
{
    return missionCompleted;
}

bool MissionManager::isShowingGoodJob()
{
    return showingGoodJob;
}

unsigned long MissionManager::getMissionCompleteElapsed()
{
    return millis() - missionCompleteTime;
}

void MissionManager::startGoodJobMessage()
{
    showingGoodJob = true;
    goodJobStartTime = millis();
}

void MissionManager::completeMission()
{
    missionCompleted = true;
    missionCompleteTime = millis();
}

void MissionManager::resetMissionComplete()
{
    missionCompleted = false;
}

int MissionManager::getMissionCount()
{
    return missionCount;
}

int MissionManager::getLastMissionCount()
{
    return lastMissionCount;
}
