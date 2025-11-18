#ifndef MISSIONMANAGER_HPP
#define MISSIONMANAGER_HPP

#include <Arduino.h>

class MissionManager
{
private:
    int missionCount;
    int lastMissionCount;
    bool missionCompleted;

public:
    MissionManager();
    void update(int increaseCount, int decreaseCount, bool touch1State, bool touch2State);
    int getMissionCount();
    int getDisplayCount(int increaseCurrentCount, int decreaseCurrentCount, bool touch1State, bool touch2State);
    bool isMissionCompleted();
    void resetMissionCompleted();
    void reset();
};

#endif
