#include "pch.h"
#include "RoomManager.h"
#include "Player.h"
#include "Room.h"
#include "BaseLevel.h"
#include "ClientPacketHandler.h"
#include "ConsoleLog.h"
#include "ChaosDunjeon.h"

RoomRef RoomManager::CreateChanel()
{
	RoomRef chanel;
	switch (MyLevelType)
	{
	case Protocol::LEVEL_TYPE_BASE:
		chanel = std::make_shared<BaseLevel>();
		break;
	case Protocol::LEVEL_TYPE_TEST:
		chanel = std::make_shared<Room>();
		break;
    case Protocol::LEVEL_TYPE_CHAOS:
        chanel = std::make_shared<ChaosDunjeon>();
        break;
	default:
		chanel = std::make_shared<Room>();
		break;
	}
	chanel->Init();
	return chanel;
}



RoomManager::RoomManager(int32 maxPlayer) : MaxPlayer(maxPlayer)
{
}

RoomManager::RoomManager(int32 maxPlayer, bool _PrivateRoom) 
    : MaxPlayer(maxPlayer), PrivateRoom(_PrivateRoom)
{
}

void RoomManager::JoinChanel(ObjectRef object, bool enter)
{
	for (auto& room : Rooms)
	{
		if(room.second == nullptr) continue;

		if (room.second->GetEnterPlayerCnt() >= MaxPlayer) continue;
        if(!PrivateRoom)
		    JoinChanel(room.first, object, enter);
        else {
            PlayerRef player = dynamic_pointer_cast<Player>(object);
            JoinChanel(player->GetCharacterID(), object, enter);
        }
            
		return;
	}
	// 여기왔으면 방을못찾아서 알아서 새롭게 넣어주셈
    if (!PrivateRoom)
	    JoinChanel(GetFirstAvailableID(), object, enter);
    else {
        PlayerRef player = dynamic_pointer_cast<Player>(object);
        JoinChanel(player->GetCharacterID(), object, enter);
    }
    
}

void RoomManager::JoinChanel(uint32 chanel, ObjectRef object, bool enter)
{
	if (Rooms[chanel] == nullptr) {
		// 해당 채널이 없을경우 채널을 생성함
		Rooms[chanel] = CreateChanel();
		//LOG("Create Chanel : " << chanel);
	}

	if (Rooms[chanel]->GetEnterPlayerCnt() >= MaxPlayer || Rooms[chanel]->GetRoomActive() == false) {
		// 접속채널 인원 초과 다른채널로 안내
        if (!PrivateRoom) {
            uint64 firstChanel = GetFirstAvailableID();
            Rooms[firstChanel] = CreateChanel();
            chanel = firstChanel;
        }
		
	}
	PlayerRef player = dynamic_pointer_cast<Player>(object);
	// 정상적으로 이동이 된걸 클라에게 알림.

	// 여기서 각종 스폰을해줌
	Rooms[chanel]->EnterRoom(player);
	if (player != nullptr) {
		// 플레이어 일경우 
		player->objectInfo->set_leveltype(MyLevelType);
		player->SetMyChanel(chanel);
		
	}
    JoinCnt++;
	//LOG("Map : " << MyLevelType <<" Join Chanel : " << chanel << " Player ObjectID : " << player->objectInfo->object_id() << " Enter : " << enter << " LevelName : " << MyLevelName);
}


uint64 RoomManager::GetFirstAvailableID()
{
	std::set<uint64> UsedIDs;
	for (const auto& pair : Rooms)
	{
		UsedIDs.insert(pair.first);
	}

	int NewID = 0;
	while (UsedIDs.find(NewID) != UsedIDs.end())
	{
		++NewID;
	}
	return NewID;
}


void RoomManager::LeaveChanel(PlayerRef player)
{
	//PlayerRef player = dynamic_pointer_cast<Player>(object);
	if(player == nullptr) return;

	//현재 접속된 채널을 가져옴.
	uint64 ObjectChanel = player->GetMyChanel();
	if(Rooms[ObjectChanel] == nullptr) return;

	// 여기서 해당 유저를 디스폰해줌
	Rooms[ObjectChanel]->LeaveRoom(player);
	
	// 플레이어 수
	uint64 enterCnt = Rooms[ObjectChanel]->GetEnterPlayerCnt();
	if (enterCnt == 0) {
		//방에 남은 사람이 없을경우 해당방은 사라짐.
		
        if (!PrivateRoom) {
            Rooms[ObjectChanel]->SetRoomActive(false);
            DoTimer(Rooms[ObjectChanel]->GetUpdateTick(), &RoomManager::RemoveRoom, ObjectChanel);
        }
        else {
            if (Rooms[ObjectChanel]->CompleteLevel) {
                // 완료 후 나가면 제거
                Rooms[ObjectChanel]->SetRoomActive(false);
                DoTimer(Rooms[ObjectChanel]->GetUpdateTick(), &RoomManager::RemoveRoom, ObjectChanel);
            }
            else {
                // TODO 완료되지않았는데 나갈경우 처리 남은시간 예약걸어두고 시간지나면 제거
                //Rooms[ObjectChanel]->SetRoomActive(false);
                shared_ptr<ChaosDunjeon> Dunjeon = dynamic_pointer_cast<ChaosDunjeon>(Rooms[ObjectChanel]);
                Rooms[ObjectChanel]->SetRoomActive(false);
                DoTimer((Dunjeon->GetCurTime() + 5) * 1000, &RoomManager::RemoveRoom, ObjectChanel);
            }
        }
		
	}
    JoinCnt--;
	//LOG("Remove Object ID : " << player->objectInfo->object_id());
}

void RoomManager::RemoveRoom(uint64 RoomID)
{
	if (Rooms.find(RoomID) == Rooms.end())
		return;


	RoomRef room = Rooms[RoomID];
	Rooms[RoomID].reset();
	Rooms[RoomID] = nullptr;
	Rooms.erase(RoomID);
	//LOG("Remove Chanel : " << RoomID);

}
void RoomManager::Broadcast(SendBufferRef sendBuffer)
{
	// 매니저에 등록된 룸접속자 모두에게 데이터를 뿌림.
	for (auto& room : Rooms)
	{
		room.second->Broadcast(sendBuffer);
	}
}
