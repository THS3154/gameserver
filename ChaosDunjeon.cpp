#include "pch.h"
#include "ChaosDunjeon.h"
#include "MapManager.h"
#include "RoomManager.h"
#include "Object.h"
#include "Player.h"
#include "SpawnTable.h"
#include "Monster.h"
ChaosDunjeon::ChaosDunjeon()
{
    dunjeonMaxTime = dunjeonMaxTime * 30;
    curDunjeonTime = dunjeonMaxTime;

    uint64 index = 0;
    // TODO ��� �ʺ��� �����͸� �����صΰ� �ҷ��� ����ҵ� �ʹ� ��������.

    vector<StructMonsterSpawn> spawnList = SpawnTable::LoadSpawnMonstersFromXML("BaseLevel.xml");

    for (auto monster : spawnList) {
        SetMonsterSpawner(index++, monster.monsterID, monster.x, monster.y, monster.z);
        SetMonsterSpawner(index++, monster.monsterID, monster.x + 100, monster.y - 70, monster.z);
        //SetMonsterSpawner(index++, monster.monsterID, monster.x + 100, monster.y + 150, monster.z);
    }

}

ChaosDunjeon::~ChaosDunjeon()
{
    //cout << "~ChaosDunjeon" << endl;
}

void ChaosDunjeon::Init()
{
    Room::Init();
    MapTick();
}

void ChaosDunjeon::UpdateTick()
{
    //MyRoom->DoAsync(&ChaosDunjeon::ProcessMonster);
    
    if (DeadMonsterCnt >= DestKillPoint) {
        StopMonsterSpawn = true;
        CompleteLevel = true;
    }

    if (bRoomActive && MyRoom != nullptr) {
        //MyRoom->DoAsync(&ChaosDunjeon::update);
        MyRoom->DoTimer(updateTick, &ChaosDunjeon::UpdateTick);
    }

}

bool ChaosDunjeon::EnterRoom(ObjectRef object)
{
    Room::EnterRoom(object);
    if (bRoomActive == false) {
        SetRoomActive(true);        // �������϶� Ȱ��ȭ
        MyRoom = GetRoomRef();
        UpdateAI();
        UpdateTick();
        PingOut();
    }
    

    return false;
}

void ChaosDunjeon::MapTick()
{
    //LOG("DunjeonTIme : " << curDunjeonTime);

    // ���������� �����ð��� ���൵�� ������.
    float progressPer = DeadMonsterCnt / (float)DestKillPoint;
    Protocol::S_DUNJEON pkt;
    pkt.set_remaintime(curDunjeonTime);
    pkt.set_progress(progressPer);
    SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
    Broadcast(sendBuffer, NoneObjectID);    // ���� �����Ȳ�� Ŭ���̾�Ʈ���� ������

    if ( 0 >= curDunjeonTime) {
        CompleteLevel = true;
        MyRoom->ClearJobs();
        MyRoom.reset();
        // TODO ���� ����
        SetRoomActive(false);   // ��Ȱ��ȭ ��

        if(_objects.size() == 0) return;

        for (auto object : _objects) {
            // ������ �ִ»�� �߹�
            if(object.second == nullptr) continue;

            object.second->posInfo->set_x(-410);
            object.second->posInfo->set_y(0);
            object.second->posInfo->set_z(180);

            Protocol::S_LEVEL_MOVE pkt;
            Protocol::LevelType levelType = Protocol::LEVEL_TYPE_TEST;

            
            Protocol::S_LEVEL_MOVE levelMove;
            auto level = GMapManager->GetRoomManager(levelType);
            levelMove.set_success(true);
            levelMove.set_leveltype(levelType);
            levelMove.set_levelname(level->GetLevelName());
            // Ŭ�󿡼� ���� ��ġ��

            object.second->movePosInfo->set_x(-410);
            object.second->movePosInfo->set_y(0);
            object.second->movePosInfo->set_z(180);
            PlayerRef player = std::static_pointer_cast<Player>(object.second);
            if(auto Session = player->session.lock()){
                auto  sendBuffer = ClientPacketHandler::MakeSendBuffer(levelMove);
                Session->Send(sendBuffer);
            }
        }

        return;
    }
    curDunjeonTime--;

    if (MyRoom != nullptr) {
        MyRoom->DoTimer(1000, &ChaosDunjeon::MapTick);
    }
    
}



bool ChaosDunjeon::IsMonsterDie(uint64 objectid)
{
    float monsterHp = _monsters[objectid]->objectInfo->hp();
    if (monsterHp > 0) {
        return false;
    }
    else {
        DeadMonsterCnt++;
        return true;
    }

}