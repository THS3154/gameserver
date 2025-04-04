#include "pch.h"
#include "MonsterUtil.h"
#include "Monster.h"
#include "ObjectUtils.h"
#include "Room.h"
#include "MonsterTable.h"

MonsterUtil::MonsterUtil()
{
}

MonsterUtil::~MonsterUtil()
{
    MonsterUtilRoom.reset();
    MonsterUtilRoom = nullptr;
}

void MonsterUtil::UpdateAI()
{

    Protocol::S_MOVES moves;

    size_t total = _monsters.size();
    if (total == 0)
        return;

    vector<uint64> monsterKeys;
    monsterKeys.reserve(total);

    for (const auto& kv : _monsters)
        monsterKeys.push_back(kv.first);

    size_t end = min(_monsterProcessIndex + MONSTER_BATCH_SIZE, monsterKeys.size());

    for (size_t i = _monsterProcessIndex; i < end; ++i) {
        uint64 monsterId = monsterKeys[i];
        MonsterRef monster = _monsters[monsterId];
        if (!monster) continue;

        MonsterState monsterState = monster->GetState();
        if (monsterState == MonsterState::IDLE || monsterState == MonsterState::MOVE) {
            // TODO 대기상태이거나 움직이는중일때만 몬스터 어그로 가능
            float x = _levelMonsterSpawnLocation[monster->SpawnerID].x;
            float y = _levelMonsterSpawnLocation[monster->SpawnerID].y;
            float AggroRange = monster->GetAggroRange();
            int searchRange = std::ceil(AggroRange / GRID_SIZE); // 서치범위
            for (auto playerid : GetNearbyPlayers(x, y, searchRange))
            {
            
                ObjectRef player = MonsterUtilRoom->GetObjects()[playerid];
                if (!player) continue;

                if (monster->Distance2D(player->posInfo->x(), player->posInfo->y()) > AggroRange)
                    continue;

                // TODO 해당 몬스터 근처에 사람검색 일단은 바로 잡힌사람으로 어그로 
                monster->SetTarget(playerid);
                break; // 지금은 처음잡은사람 바로 나감
            }
        }
        else if (monsterState == MonsterState::AGGRO || monsterState == MonsterState::ATTACK)
        {
            // TODO 어그로 상태이거나 공격상태일때만 확인 스폰위치에서 타겟이 거리를 벗어나면 어그로 잃음
            uint64 spawnerID = monster->SpawnerID;
            float x = _levelMonsterSpawnLocation[spawnerID].x;
            float y = _levelMonsterSpawnLocation[spawnerID].y;
            //Vector3 SpawnerVector(x, y, 0);
            float AggroRange = monster->GetAggroRange();
            uint64 TargetID = monster->GetTarget();
            if (TargetID == 0) {
                // TODO 타겟아이디값이없는데 상태값이 이상함으로 상태값을 돌려줌
                monster->LostTarget();
            }

        }

        if (monsterState != MonsterState::ATTACK)
        {
            // 일단 움직여
            // TODO 여기서만 클라에게 패킷을 보냄
            Protocol::PosInfo* pos = moves.add_posinfo();
            if (Protocol::PosInfo* posinfo = SendMonsterMove(monster)) {
                if (posinfo != nullptr) {
                    pos->CopyFrom(*posinfo);
                }
            }
        }
    }

    _monsterProcessIndex = (end >= monsterKeys.size()) ? 0 : end;
    if (moves.posinfo_size() > 0)
    {
        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(moves);
        if (MonsterUtilRoom)
            MonsterUtilRoom->Broadcast(sendBuffer, 0);

    }

    
}


//서버에 스포너 위치 설정
void MonsterUtil::SetMonsterSpawner(uint64 spawnerID, uint64 monsterType, float x, float y, float z)
{
    SpawnLocation location;
    location.x = x;
    location.y = y;
    location.z = z;
    location.yaw = 1;
    _levelMonsterSpawn.insert({ spawnerID, monsterType });
    _levelMonsterSpawnLocation.insert({ spawnerID ,location });

    //몬스터를 생성해서 서버에 저장시킴
    MonsterRef monster = ObjectUtils::CreateMonster(spawnerID, x, y, z);
    _monsters[monster->objectInfo->object_id()] = monster;
}

void MonsterUtil::MonsterSpawn(vector<uint64> spawnerIDs)
{
    if (StopMonsterSpawn) return;

    for (auto spawnerID : spawnerIDs)
    {
        MonsterSpawn(spawnerID);
    }
}

void MonsterUtil::MonsterSpawn(uint64 spawnerID)
{

    Protocol::S_SPAWN pkt;

    uint64 MonsterType = _levelMonsterSpawn[spawnerID];
    SpawnLocation SpawnerPos = _levelMonsterSpawnLocation[spawnerID];
    uint64 RegenTimer = MonsterTable::MonsterInfos[MonsterType].regentime;	//리젠시간
    MonsterRef monster = ObjectUtils::CreateMonster(spawnerID, SpawnerPos.x, SpawnerPos.y, SpawnerPos.z);

    Protocol::ObjectInfo* monster_objectInfo = pkt.add_objectinfos();
    monster_objectInfo->CopyFrom(*monster->objectInfo);

    SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
    
    if(MonsterUtilRoom)
        MonsterUtilRoom->DoTimer(RegenTimer, &Room::MonsterBroadcast, sendBuffer, monster);
}


void MonsterUtil::MonsterDeSpawn(vector<uint64> objectIDs)
{
    // 클라이언트에게 일단 디스폰을 날림.
    Protocol::S_DESPAWN despawnPkt;
    for (auto objectID : objectIDs) {
        Protocol::DespawnList* despawn = despawnPkt.add_despawns();
        despawn->set_id(objectID);
        _monsters.erase(objectID);				//서버에서 해당 몬스터를 가지고 있지않음.
        //LOG("DeSpawn : " << objectID);
    }
    SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(despawnPkt);

    if (MonsterUtilRoom)
        MonsterUtilRoom->Broadcast(sendBuffer, 0);

}

void MonsterUtil::MonsterHit(uint64 objectID, float Damage)
{
    MonsterRef monster = _monsters[objectID];

    if (monster == nullptr)	return;

    Protocol::ObjectInfo* objectinfo = monster->objectInfo;
    monster->OnHit(Damage);

}
Protocol::PosInfo* MonsterUtil::SendMonsterMove(MonsterRef monster)
{
    if (monster == nullptr) return nullptr;
    uint64 objectid = monster->objectInfo->object_id();
    bool NoReturn = false;
    Protocol::PosInfo* pos = nullptr;
    // 움직이기전 그리드 데이터를 삭제
    RemoveMonsterToGrid(objectid, monster->posInfo->x(), monster->posInfo->y());
    if (monster->GetState() == MonsterState::AGGRO)
    {
        ObjectRef object = MonsterUtilRoom->GetObjects()[monster->GetTarget()];
        if (object != nullptr) {
            // 몬스터 어글자 위치 설정
            monster->SetDestPos(monster, object);

            if (object->Distance2D(_levelMonsterSpawnLocation[monster->SpawnerID].x, _levelMonsterSpawnLocation[monster->SpawnerID].y) >= monster->GetAggroRange()) {
                // 어그로 범위를 벗어남
                monster->LostTarget();
            }
            else {
                // 어그로 범위 안에있을경우 움직임
                pos = monster->ServerMove(monster, (MonstertTickTime  / (float)1000));
            }

        }
        else {
            monster->LostTarget();
        }

    }
    else
    {
        if (monster->bmovePos)
        {
            // 몬스터 움직임 예상
            monster->VoidServerMove(monster, (MonstertTickTime / (float)1000));
        }
        else {
            // 몬스어 어슬렁어슬렁 위치 설정
            monster->SetDestPos(monster, _levelMonsterSpawnLocation);
            pos = new Protocol::PosInfo();
            pos->CopyFrom(monster->objectInfo->posinfo());
        }
    }

    //움직인 후 위치 값을 그리드 데이터에 추가
    AddMonsterToGrid(objectid, monster->posInfo->x(), monster->posInfo->y());
    if (pos == nullptr) return nullptr;
    return pos;
}


//몬스터 주변에 플레이어가 있는지 확인
std::vector<uint64> MonsterUtil::GetNearbyPlayers(float monsterX, float monsterY, int searchRange)
{
	int gridX = monsterX / GRID_SIZE;
	int gridY = monsterY / GRID_SIZE;

	std::vector<uint64_t> nearbyPlayers;
	for (int dx = -searchRange; dx <= searchRange; dx++) {
		for (int dy = -searchRange; dy <= searchRange; dy++) {
			auto it = GridPlayers.find({ gridX + dx, gridY + dy });
			if (it != GridPlayers.end()) {
				nearbyPlayers.insert(nearbyPlayers.end(), it->second.begin(), it->second.end());
			}
		}
	}
	return nearbyPlayers;
}


bool MonsterUtil::IsMonsterDie(uint64 objectid)
{
    float monsterHp = _monsters[objectid]->objectInfo->hp();
    if (monsterHp > 0) {
        //해당유닛 생존
        return false;
    }
    else {
        //해당유닛 사망
        return true;
    }

}

bool MonsterUtil::DeSpawnMonster(uint64 objectid)
{
    return false;
}


void MonsterUtil::AddMonsterToGrid(uint64 monsterID, float x, float y)
{
	int gridX = x / GRID_SIZE;
	int gridY = y / GRID_SIZE;
	GridMonsters[{gridX, gridY}].push_back(monsterID);
	
}

void MonsterUtil::RemoveMonsterToGrid(uint64 monsterID, float x, float y)
{
	int gridX = x / GRID_SIZE;
	int gridY = y / GRID_SIZE;

	auto& monsters = GridMonsters[{gridX, gridY}];
	monsters.erase(std::remove(monsters.begin(), monsters.end(), monsterID), monsters.end());
}



