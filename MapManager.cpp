#include "pch.h"
#include "RoomManager.h"
#include "Player.h"
#include "MapManager.h"
MapManagerRef GMapManager = make_shared<MapManager>();

MapManager::MapManager()
{
	CreateMap(Protocol::LEVEL_TYPE_BASE, "ThirdPersonMap", 40, false);	// ���̽���
    CreateMap(Protocol::LEVEL_TYPE_TEST, "TestMap", MaxPeople, false);	// �׽�Ʈ��
    CreateMap(Protocol::LEVEL_TYPE_CHAOS, "ChaosDunjeon", MaxPeople, true);	// ī��������
}

MapManager::~MapManager()
{
}


bool MapManager::JoinMap(Protocol::LevelType levelType, ObjectRef object, bool enter)
{
	RoomManagerRef joinMap = Levels[levelType];
	if (joinMap == nullptr) {
		//��ϵ� ���� ������� �����ϰ� ���� ��Ŵ.
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

		//ä���� ��������.
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
	// �̹� ������ �������� ������������.
	// �ѹ� ������ �������� ���� �� ���Ű� ��������.
	// ���� RoomManager���� ���� ó���ϴ��� ��.
	if(Levels[levelType] != nullptr) return;	
	if(maxPeople == 0) maxPeople = MaxPeople;	//���°��� ������ �⺻������ ������ �ο����� ����
	RoomManagerRef map = make_shared<RoomManager>(maxPeople);
	map->SetLevelType(levelType);
	map->SetLevelName(levelName);
	Levels[levelType] = map;
	LevelNames[levelType] = levelName;
}

void MapManager::CreateMap(Protocol::LevelType levelType, string levelName, int32 maxPeople, bool privateRoom)
{
    if (Levels[levelType] != nullptr) return;
    if (maxPeople == 0) maxPeople = MaxPeople;	//���°��� ������ �⺻������ ������ �ο����� ����
    RoomManagerRef map = make_shared<RoomManager>(maxPeople, privateRoom);
    map->SetLevelType(levelType);
    map->SetLevelName(levelName);
    Levels[levelType] = map;
    LevelNames[levelType] = levelName;
}
