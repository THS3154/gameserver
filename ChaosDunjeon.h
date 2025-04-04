#pragma once
#include "Room.h"
class ChaosDunjeon :
    public Room
{

public:
    ChaosDunjeon();
    ~ChaosDunjeon();

    void Init() override;
    void UpdateTick() override;
    virtual bool EnterRoom(ObjectRef object) override;
    void MapTick();
    int GetCurTime(){return curDunjeonTime;}
private:
    // ∏ÛΩ∫≈Õ
    int curDunjeonTime = 0;
    uint64 DeadMonsterCnt = 0;
    int dunjeonMaxTime = 1;
    int DestKillPoint = 10;
    virtual bool IsMonsterDie(uint64 objectid) override;
};

