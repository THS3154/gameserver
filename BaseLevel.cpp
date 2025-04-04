#include "pch.h"
#include "BaseLevel.h"
#include "ObjectUtils.h"
#include "Monster.h"
#include "MonsterTable.h"
#include "ClientPacketHandler.h"
#include "SpawnTable.h"

BaseLevel::BaseLevel()
{
	uint64 index = 0;
    vector<StructMonsterSpawn> spawnList = SpawnTable::LoadSpawnMonstersFromXML("BaseLevel.xml");
    
    for (auto monster : spawnList) {
        SetMonsterSpawner(index++, monster.monsterID, monster.x, monster.y, monster.z);
    }

}

BaseLevel::~BaseLevel()
{
	//cout << "~BaseLevel" << endl;

}

void BaseLevel::Init()
{
	Room::Init();

}

void BaseLevel::UpdateTick()
{
    
	if (bRoomActive && MyRoom != nullptr) {
        //MyRoom->DoAsync(&BaseLevel::ProcessMonster);
		//MyRoom->DoTimer(updateTick, &BaseLevel::UpdateTick);
	}
		

}


