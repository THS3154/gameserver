#pragma once
#include "JobQueue.h"

class MapManager : public JobQueue
{
public:
	MapManager();
	virtual ~MapManager();

public:
    unordered_map<uint64, string> GetLevelNames(){return LevelNames;}

public:
	bool JoinMap(Protocol::LevelType levelType, ObjectRef object, bool enter);
	bool LeaveMap(ObjectRef object);
public:
	void SetMaxPeople(int32	maxPeople);

	RoomRef GetRoom(ObjectRef object);
	RoomManagerRef GetRoomManager(ObjectRef object);
	RoomManagerRef GetRoomManager(uint64 index);
private:
    void CreateMap(Protocol::LevelType levelType, string levelName, int32 maxPeople);
    void CreateMap(Protocol::LevelType levelType, string levelName, int32 maxPeople, bool privateRoom);

private:
	int32 MaxPeople = 30;	// 한채널 최대 접속가능인원
	unordered_map<uint64, RoomManagerRef> Levels;
	unordered_map<uint64, string> LevelNames;
};

extern MapManagerRef GMapManager;
