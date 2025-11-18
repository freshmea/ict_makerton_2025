#include "MissionManager.hpp"

MissionManager::MissionManager()
{
    missionCount = 0;
    lastMissionCount = 0;
    missionCompleted = false;
}

void MissionManager::update(int increaseCount, int decreaseCount, bool touch1State, bool touch2State)
{
    int finalMissionCount = increaseCount - decreaseCount;
    if (finalMissionCount < 0)
        finalMissionCount = 0;

    int displayCount = getDisplayCount(0, 0, touch1State, touch2State);

    // 미션 완료 조건 체크
    if (!missionCompleted && lastMissionCount >= 1 && displayCount == 0)
    {
        missionCompleted = true;
    }

    lastMissionCount = displayCount;
    missionCount = finalMissionCount;
}

int MissionManager::getMissionCount()
{
    return missionCount;
}

int MissionManager::getDisplayCount(int increaseCurrentCount, int decreaseCurrentCount, bool touch1State, bool touch2State)
{
    int baseCount = missionCount;

    if (touch2State)
    {
        return baseCount + increaseCurrentCount;
    }
    else if (touch1State)
    {
        int result = baseCount - decreaseCurrentCount;
        return result < 0 ? 0 : result;
    }
    else
    {
        return baseCount;
    }
}

bool MissionManager::isMissionCompleted()
{
    return missionCompleted;
}

void MissionManager::resetMissionCompleted()
{
    missionCompleted = false;
}

void MissionManager::reset()
{
    missionCount = 0;
    lastMissionCount = 0;
    missionCompleted = false;
}
