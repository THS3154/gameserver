#include "pch.h"
#include "RoomManager.h"
#include "Player.h"
#include "MapManager.h"
MapManagerRef GMapManager = make_shared<MapManager>();

MapManager::MapManager()
{
	CreateMap(Protocol::LEVEL_TYPE_BASE, "ThirdPersonMap", 40, false);	// 베이스맵
    CreateMap(Protocol::LEVEL_TYPE_TEST, "TestMap", MaxPeople, false);	// 테스트맵
    CreateMap(Protocol::LEVEL_TYPE_CHAOS, "ChaosDunjeon", MaxPeople, true);	// 카오스던전
}

MapManager::~MapManager()
{
}


bool MapManager::JoinMap(Protocol::LevelType levelType, ObjectRef object, bool enter)
{
	RoomManagerRef joinMap = Levels[levelType];
	if (joinMap == nullptr) {
		//등록된 맵이 없을경우 생성하고 접속 시킴.
		CreateMap(levelType, LevelNames[levelType],0);
	}
	joinMap->JoinChanel(object, enter);
	return true;
}

bool MapManager::LeaveMap(ObjectRef object)
{
	if (auto player = dynamic_pointer_cast<Player>(object)) {
		uint64 map = player->objectInfo->leveltype();
		RoomManagerRef roomManager = GetRoomManager(object);
		if(roomManager == nullptr) return false;

		//채널을 나가게함.
		roomManager->LeaveChanel(player);
		return true;
	}

	return false;
}

void MapManager::SetMaxPeople(int32 maxPeople)
{
	MaxPeople = maxPeople;
}

RoomRef MapManager::GetRoom(ObjectRef object)
{
	if (auto player = dynamic_pointer_cast<Player>(object)) {
		uint64 map = player->objectInfo->leveltype();
		uint64 chanel = player->objectInfo->chanel();
		
		return Levels[map]->Rooms[chanel];
	}
	else {
		return nullptr;
	}
	
}

RoomManagerRef MapManager::GetRoomManager(ObjectRef object)
{
	if (auto player = dynamic_pointer_cast<Player>(object)) {
		uint64 map = player->objectInfo->leveltype();
		return Levels[map];
	}
	else {
		return nullptr;

	}
}

RoomManagerRef MapManager::GetRoomManager(uint64 index)
{
	return Levels[index];
}

void MapManager::CreateMap(Protocol::LevelType levelType, string levelName, int32 maxPeople)
{
	// 이미 생성된 맵정보면 생성하지않음.
	// 한번 생성된 맵정보는 생성 후 제거가 되지않음.
	// 내부 RoomManager에서 따로 처리하던가 함.
	if(Levels[levelType] != nullptr) return;	
	if(maxPeople == 0) maxPeople = MaxPeople;	//들어온값이 없으면 기본적으로 설정된 인원수로 지정
	RoomManagerRef map = make_shared<RoomManager>(maxPeople);
	map->SetLevelType(levelType);
	map->SetLevelName(levelName);
	Levels[levelType] = map;
	LevelNames[levelType] = levelName;
}

void MapManager::CreateMap(Protocol::LevelType levelType, string levelName, int32 maxPeople, bool privateRoom)
{
    if (Levels[levelType] != nullptr) return;
    if (maxPeople == 0) maxPeople = MaxPeople;	//들어온값이 없으면 기본적으로 설정된 인원수로 지정
    RoomManagerRef map = make_shared<RoomManager>(maxPeople, privateRoom);
    map->SetLevelType(levelType);
    map->SetLevelName(levelName);
    Levels[levelType] = map;
    LevelNames[levelType] = levelName;
}
