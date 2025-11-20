#ifndef MISSIONMANAGER_HPP
#define MISSIONMANAGER_HPP

#include <Arduino.h>

class MissionManager
{
private:
    bool missionCompleted;

public:
    MissionManager();
    void update(int missionCount, int &lastmissionCount);
    bool isMissionCompleted();
    void resetMissionCompleted();
};

#endif
