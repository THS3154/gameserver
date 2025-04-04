#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "Object.h"
#include "Creature.h"
#include "GameSession.h"
#include "Monster.h"
#include "ObjectUtils.h"
#include "MonsterTable.h"
#include "ClientPacketHandler.h"
#include "Utils.h"
#include "PlayerLevel.h"
#include "CharacterDB.h"
#include "PlayerGold.h"
#include "DBManager.h"
#include "ItemUtils.h"
#include "MapManager.h"


Room::Room()
{
}

Room::~Room()
{
}

bool Room::EnterRoom(ObjectRef object)
{
	bool success = AddObject(object);

	// 입장하는 클라이언트에게 입장됐다고 알림.
	if(auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::S_ENTER_GAME enterPkt;
		enterPkt.set_success(true);
		Protocol::ObjectInfo* playerInfo = enterPkt.mutable_objectinfo();
		playerInfo->CopyFrom(*player->objectInfo);
		//LOG("EnterRoom POS : X : " << player->posInfo->x());
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(enterPkt);
		if (auto session = player->session.lock()) {

			session->Send(sendBuffer);
            
            uint64 CharacterID = session->myCharacterDBID[player->objectInfo->object_id()];
            // TODO 인벤토리 요청 / 장착중인 장비 요청.
            if (player->Inventory->GetLoadInventory() == false) {
                // TODO 인벤토리 DB서버에서 요청
                CharacterDB::GetPlayerInventory(session, CharacterID);  // 인벤토리 요청
            }
            else {
                // TODO 메모리에 등록된 인벤토리 데이터 전송
                Protocol::S_LOADINVENTORY inventoryPkt = player->Inventory->SendInventoryPkt();
                SendBufferRef sendInventoryBuffer = ClientPacketHandler::MakeSendBuffer(inventoryPkt);
                session->Send(sendInventoryBuffer);
            }
            
            if (player->Equipment->GetLoadEquipment() == false) {
                CharacterDB::GetPlayerEquipment(session, CharacterID);  // 장비 요청
            }
            else {
                Protocol::S_LOADEQUIPMENT loadEquPkt = player->Equipment->ClientSendEquipmentPkt();
                Protocol::ObjectInfo* objectinfo = loadEquPkt.mutable_objectinfo();
                objectinfo->CopyFrom(*player->objectInfo);
                SendBufferRef sendEquipmentBuffer = ClientPacketHandler::MakeSendBuffer(loadEquPkt);
                //Broadcast(sendEquipmentBuffer, NotObjectID);    

                RoomRef room = GetRoomRef();
                room->DoAsync(&Room::Broadcast, sendEquipmentBuffer, NoneObjectID); // 내 장착정보를 전부에게 알림
            }
		}
	}
    

	// 기존 접속유저 정보를 접속유저에게 보냄
	{
		Protocol::S_SPAWN spawnPkt;
		for (auto& item : _objects)
		{
            if(item.second == nullptr) continue;
			if(item.second->IsPlayer() == false) continue;
            
			Protocol::ObjectInfo* objectInfo = spawnPkt.add_objectinfos();
			objectInfo->CopyFrom(*item.second->objectInfo);
		}


		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		if (auto player = dynamic_pointer_cast<Player>(object))
			if (auto session = player->session.lock())
				session->Send(sendBuffer);
	}

	// 생성된 몬스터정보를 넘겨줌
	{
		Protocol::S_SPAWN pkt;
		{
			int i = 0;
			for (auto item : _monsters) {
				i++;
				Protocol::ObjectInfo* monster_objectInfo = pkt.add_objectinfos();
				MonsterRef monster = item.second;
				monster_objectInfo->CopyFrom(*monster->objectInfo);
				//LOG("Monster HP : " << monster->objectInfo->hp());
			}

		}
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		if (auto player = dynamic_pointer_cast<Player>(object))
			if (auto session = player->session.lock())
				session->Send(sendBuffer);
	}

	//다른 유저에게 접속했다고 알림
	{
		Protocol::S_SPAWN spawnPkt;
		Protocol::ObjectInfo* playerInfo = spawnPkt.add_objectinfos();
		playerInfo->CopyFrom(*object->objectInfo);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
        
		Broadcast(sendBuffer, object->objectInfo->object_id());
	}

	return true;
}

bool Room::LeaveRoom(ObjectRef object)
{
	if (object == nullptr)
		return false;

	const uint64 objectid = object->objectInfo->object_id();
	bool success = RemoveObject(objectid);

	// 게임 떠났다고 서버에 알림
	{
		Protocol::S_LEAVE_GAME leaveGamePkt;
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(leaveGamePkt);

		if (auto player = dynamic_pointer_cast<Player>(object))
            if (auto session = player->session.lock()) {
				session->Send(sendBuffer);
                
                // 채널이동 및 떠날시 데이터 저장
                uint64 CharacterID = session->myCharacterDBID[player->objectInfo->object_id()];
                player->Inventory->UpdateInventory(CharacterID);        // 인벤토리 서버 업데이트
                player->Equipment->DBSendEquipment(CharacterID);           // 장비창 서버 업데이트
            }
	}

	// 다른유저에게 해당 유저 디스폰하라고 명령
	{
		Protocol::S_DESPAWN despawnPkt;
        Protocol::DespawnList* despawn = despawnPkt.add_despawns();
        despawn->set_id(objectid);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectid);
		if (auto player = dynamic_pointer_cast<Player>(object))
			if (auto session = player->session.lock())
				session->Send(sendBuffer);
	}

	return true;
}

bool Room::HandleEnterPlayer(PlayerRef player)
{
	return EnterRoom(player);;
}

bool Room::HandleLeavePlayer(PlayerRef player)
{

	return LeaveRoom(player);
}

void Room::HandleMove(Protocol::C_MOVE pkt)
{

	const uint64 objectId = pkt.posinfo().object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	
	// 적용
	ObjectRef& player = _objects[objectId];

	if (player->IsPlayer())
	{
		// 플레이어 그리드 연산

		// 현재 위치한 그리드 좌표
		int oldGridX = player->posInfo->x() / GRID_SIZE;
		int oldGridY = player->posInfo->y() / GRID_SIZE;

		// 새로운 위치의 그리드 좌표
		int newGridX = pkt.posinfo().x() / GRID_SIZE;
		int newGridY = pkt.posinfo().y() / GRID_SIZE;


		if (oldGridX != newGridX || oldGridY != newGridY)
		{
			//무브패킷이 유저일경우 그리드 데이터 업데이트
			RemovePlayerToGrid(objectId, player->posInfo->x(), player->posInfo->y());
			AddPlayerToGrid(objectId, pkt.posinfo().x(), pkt.posinfo().y());
		}
	}
	
	 

	player->posInfo->CopyFrom(pkt.posinfo());

	// 이동 
	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_posinfo();
			info->CopyFrom(pkt.posinfo());
		}

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
		RoomRef room = GetRoomRef();
		room->DoAsync(&Room::Broadcast, sendBuffer, player->objectInfo->object_id());
		//Broadcast(sendBuffer);
	}
}

void Room::HandleSkill(Protocol::C_SKILL pkt)
{

	uint64 objectid = pkt.objectinfo().object_id();
	if (_objects.find(objectid) != _objects.end()) {
		//플레이어 공격
		ObjectRef object = _objects[objectid];
		PlayerRef player = dynamic_pointer_cast<Player>(object);
		if (player) {
			MyRoom->DoAsync(&Room::PlayerAttack,player, pkt);
		}
		return;
	}

	if (_monsters.find(objectid) != _monsters.end()) {
		//몬스터 공격

	}
	
		
}



void Room::UpdateAI()
{
    MonsterUtil::UpdateAI();
    if (IsRoomActive()) {
        MyRoom->DoTimer(MonsterUtil::GetMonsterTickTime(), &Room::UpdateAI);
    }
}

void Room::PlayerAttack(PlayerRef player, Protocol::C_SKILL pkt)
{
	vector<uint64> DeSpawnObjectIDs;
	vector<uint64> SpawnIDs;
	uint64 objectid = player->objectInfo->object_id();
	
	Protocol::S_SKILL sendPkt;
	sendPkt.set_caster_id(objectid);
	sendPkt.set_skill_id(pkt.skill_id());

	
	FSkillData SkillInfo = player->GetSkilIData(pkt.skill_id());
	// 플레이어 현재 위치
	float posX = player->posInfo->x();
	float posY = player->posInfo->y();
	int searchRange = std::ceil(SkillInfo.Range / GRID_SIZE); // 서치범위

	// 내가 사용한 스킬 범위 내 스캔된 몬스터
	vector<uint64> SearchMonsters = GetNearbyMonsters(posX, posY, searchRange);
	//LOG("SEARCH MONSTERS : " << SearchMonsters.size());
	bool bMonsterDie = false;
	for (auto monsterid : SearchMonsters) {

		
		MonsterRef monster = _monsters[monsterid];
		if(monster == nullptr) continue;
		bool bCritical = player->IsCritical();
		float Damage = player->Hit(pkt.skill_id(), bCritical);

		Protocol::ObjectInfo* objectinfo = monster->objectInfo;
		uint64 objectindex = objectinfo->object_id();
		// TODO 일단 몬스터와 직선거리를 통해서 범위안에있으면 공격하고 아니면 패스
		if(player->Distance2D(monster->posInfo->x(), monster->posInfo->y()) > SkillInfo.Range)
			continue;
		
		MonsterHit(objectindex, Damage);
		Protocol::SkillHitResult* r = sendPkt.add_hit_results();
		r->set_damage(Damage);
		r->set_object_id(objectindex);
		r->set_is_critical(bCritical);

		if (IsMonsterDie(objectindex)) {
			bMonsterDie = true;
            //DeadMonsterCnt++;
			// TODO 디스폰 objectid값과 스포너값을 추출해서 벡터로 각각 가지고있다가 디스폰과 스폰에 넘겨줌
			DeSpawnObjectIDs.push_back(objectindex);
			SpawnIDs.push_back(monster->SpawnerID);

			Protocol::ObjectInfo* monsterobj = monster->objectInfo;
			RemoveMonsterToGrid(monsterobj->object_id(), monsterobj->posinfo().x(), monsterobj->posinfo().y());
		}

	}
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(sendPkt);
	MyRoom->DoAsync(&Room::Broadcast, sendBuffer, NoneObjectID);
	if (bMonsterDie) {
		//LOG("DeSpawnObject : " << DeSpawnObjectIDs.size() << " SpawnObject : " << SpawnIDs.size());
		int exp = 0;
		int gold = 0;
        bool bGetItem = false;
		Protocol::S_DESPAWN despawnPkt;
		for (auto objectID : DeSpawnObjectIDs) {
			MonsterRef monster = _monsters[objectID];
            Protocol::DespawnList* despawn = despawnPkt.add_despawns();
            despawn->set_id(objectID);
            if (auto session = player->session.lock()) {
                despawn->set_dropowner(session->myCharacterDBID[player->objectInfo->object_id()]);
            }
            // 몬스터 드랍 처리
            for (auto drop : monster->DropTable) {
                
                ServerItemData* item = ItemUtils::GetItem(drop.code);
                if (item) {
                    // drop.rate 아이템 드랍확률
                    float GetRand = ItemUtils::RandomFloat(); // 독립시행
                    if (GetRand <= drop.rate) {
                        // 아이템 드랍 패킷저장
                        despawn->add_itemcode(drop.code);
                        bGetItem = true;
                        player->Inventory->DropItem(*item);
                        LOG("Drop Object : " << objectID << " ItemCode : " << drop.code << " Drop Rate : " << drop.rate << " ItemName : " << item->Name);
                    }

                }
            }
            
            
			
			// TODO 경험치 획득증가 추가되면 여기서 추가해주면됨.
			gold += monster->objectInfo->gold();	// 골드 모아서 전달
			exp += monster->objectInfo->curexp();	// 경험치 모아서 전달
			_monsters.erase(objectID);				// 서버에서 해당 몬스터를 가지고 있지않음.
		}


        // 임의로 아이템 얻었을때는 인벤토리값을 다보내줌...
        if (bGetItem) {
            if (auto session = player->session.lock()) {
                Protocol::S_LOADINVENTORY inventoryPkt = player->Inventory->SendInventoryPkt();
                SendBufferRef sendInventoryBuffer = ClientPacketHandler::MakeSendBuffer(inventoryPkt);
                session->Send(sendInventoryBuffer);
            }
        }


		SendBufferRef desendBuffer = ClientPacketHandler::MakeSendBuffer(despawnPkt);
		MyRoom->DoAsync(&Room::Broadcast, desendBuffer, NoneObjectID);

		MonsterSpawn(SpawnIDs);				//스폰 대기열 추가
		
		
		PlayerLevel::AddExp(player, exp);
		PlayerGold::UpdateGold(player, gold, LOG_GOLD_GET_MONSTER);
	}

}


bool Room::RemoveObject(uint64 objectid)
{
	if (_objects.find(objectid) == _objects.end())
		return false;

	ObjectRef player = _objects[objectid];
	player->room = std::move(weak_ptr<Room>());

	_objects.erase(objectid);
	return true;
}

void Room::PingOut()
{
    std::vector<ObjectRef> toRemove;

    for (auto& object : _objects) {
        if (ObjectRef obj = object.second) {
            if (Utils::Millis() - obj->pingTime > 5000 && obj->pingTime != 0) {
                toRemove.push_back(obj);
            }
        }
    }

    for (auto& obj : toRemove) {
        GMapManager->LeaveMap(obj);
    }

    if (bRoomActive) {
        MyRoom->DoTimer(1000, &Room::PingOut);
    }
}


void Room::UpdateTick()
{

    if (bRoomActive) {
		//MyRoom->DoTimer(updateTick, &Room::UpdateTick);
    }
}

uint64 Room::GetEnterPlayerCnt()
{
	uint64 cnt = 0;
	for (auto& item : _objects) {
		//플레이어 이면 값을 증가
		if (auto player = dynamic_pointer_cast<Player>(item.second)) cnt++;
	}
	return cnt;
}

void Room::Init()
{
	MyRoom = GetRoomRef();
    MonsterUtil::MonsterUtilRoom = GetRoomRef();
	UpdateTick();
    UpdateAI();
    PingOut();
}



RoomRef Room::GetRoomRef()
{
	try
	{
		RoomRef ptr = static_pointer_cast<Room>(shared_from_this());
		//LOG("SUCCESS: shared_from_this() called. use_count: " << ptr.use_count());
		return ptr;
	}
	catch (const std::bad_weak_ptr& e)
	{
		LOG("ERROR: shared_from_this() failed! " << e.what());
		return nullptr;
	}
}




bool Room::AddObject(ObjectRef obejct)
{

    if (_objects.find(obejct->objectInfo->object_id()) != _objects.end()) {
        if (_objects[obejct->objectInfo->object_id()] != nullptr) {
            return false;
        }
        else {
            _objects.erase(obejct->objectInfo->object_id());
        }
		
    }

	_objects.insert(make_pair(obejct->objectInfo->object_id(), obejct));

	obejct->room = std::move(GetRoomRef());
	return true;
}




/**************************************
		몬스터 사냥 습득 정보
**************************************/
int Room::GetGainExp(vector<uint64> objectIDs)
{
	// 플레이어가 사냥한 몬스터 경험치 모아서 전달
	int GainExp = 0;
	for (auto objectID : objectIDs) {
		MonsterRef monster = _monsters[objectID];
		// TODO 경험치 획득증가 추가되면 여기서 추가해주면됨.

		GainExp += monster->objectInfo->curexp();
	}

	return GainExp;
}

int Room::GetGainGold(vector<uint64> objectIDs)
{
	// 플레어가 사냥한 몬스터 골드 모아서 전달
	// 플레이어가 사냥한 몬스터 경험치 모아서 전달
	int GainGold = 0;
	for (auto objectID : objectIDs) {
		MonsterRef monster = _monsters[objectID];
		// TODO 경험치 획득증가 추가되면 여기서 추가해주면됨.

		GainGold += monster->objectInfo->gold();
	}

	return GainGold;
}



void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	//cout << "Room Broadcast Size : " << _objects.size() << endl;
	for (auto& object : _objects)
	{
        if(object.second == nullptr) continue;

		PlayerRef player = dynamic_pointer_cast<Player>(object.second);

		if (player == nullptr)
			continue;

		if (player->objectInfo->object_id() == exceptId)
			continue;

		if (GameSessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}

}



void Room::MonsterBroadcast(SendBufferRef sendBuffer, MonsterRef monster)
{
	for (auto& object : _objects)
	{
        if (object.second == nullptr) continue;

		PlayerRef player = dynamic_pointer_cast<Player>(object.second);

		if (player == nullptr)
			continue;

		if (GameSessionRef session = player->session.lock()) {
			session->Send(sendBuffer);
			//cout << "MonsterBroadcast" << endl;
		}
	}
	_monsters[monster->objectInfo->object_id()] = monster;

	

}

