#ifndef MISSIONMANAGER_H
#define MISSIONMANAGER_H

#include <Arduino.h>

class MissionManager
{
private:
    int missionCount;
    bool missionCompleted;
    unsigned long missionCompleteTime;
    int lastMissionCount;
    bool showingGoodJob;
    unsigned long goodJobStartTime;

public:
    MissionManager();

    void update(int touch1LastBeepCount, int touch2LastBeepCount, int touch1BeepCount, int touch2BeepCount, bool touch1State, bool touch2State);
    int calculateCurrentMissionCount(int touch1LastBeepCount, int touch2LastBeepCount, int touch1BeepCount, int touch2BeepCount, bool touch1State, bool touch2State);

    bool isMissionCompleted();
    bool isShowingGoodJob();
    unsigned long getMissionCompleteElapsed();

    void startGoodJobMessage();
    void completeMission();
    void resetMissionComplete();

    int getMissionCount();
    int getLastMissionCount();
};

#endif
