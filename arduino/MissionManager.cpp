#include "MissionManager.hpp"

MissionManager::MissionManager()
{
    missionCompleted = false;
}

void MissionManager::update(int missionCount, int &lastMissionCount)
{
    // 미션 완료 조건 체크
    if (!missionCompleted && lastMissionCount >= 1 && missionCount == 0)
    {
        missionCompleted = true;
    }
    lastMissionCount = missionCount;
}

bool MissionManager::isMissionCompleted()
{
    return missionCompleted;
}

void MissionManager::resetMissionCompleted()
{
    missionCompleted = false;
}