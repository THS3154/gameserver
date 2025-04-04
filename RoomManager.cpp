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
	// ��������� ������ã�Ƽ� �˾Ƽ� ���Ӱ� �־��ּ�
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
		// �ش� ä���� ������� ä���� ������
		Rooms[chanel] = CreateChanel();
		//LOG("Create Chanel : " << chanel);
	}

	if (Rooms[chanel]->GetEnterPlayerCnt() >= MaxPlayer || Rooms[chanel]->GetRoomActive() == false) {
		// ����ä�� �ο� �ʰ� �ٸ�ä�η� �ȳ�
        if (!PrivateRoom) {
            uint64 firstChanel = GetFirstAvailableID();
            Rooms[firstChanel] = CreateChanel();
            chanel = firstChanel;
        }
		
	}
	PlayerRef player = dynamic_pointer_cast<Player>(object);
	// ���������� �̵��� �Ȱ� Ŭ�󿡰� �˸�.

	// ���⼭ ���� ����������
	Rooms[chanel]->EnterRoom(player);
	if (player != nullptr) {
		// �÷��̾� �ϰ�� 
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

	//���� ���ӵ� ä���� ������.
	uint64 ObjectChanel = player->GetMyChanel();
	if(Rooms[ObjectChanel] == nullptr) return;

	// ���⼭ �ش� ������ ��������
	Rooms[ObjectChanel]->LeaveRoom(player);
	
	// �÷��̾� ��
	uint64 enterCnt = Rooms[ObjectChanel]->GetEnterPlayerCnt();
	if (enterCnt == 0) {
		//�濡 ���� ����� ������� �ش���� �����.
		
        if (!PrivateRoom) {
            Rooms[ObjectChanel]->SetRoomActive(false);
            DoTimer(Rooms[ObjectChanel]->GetUpdateTick(), &RoomManager::RemoveRoom, ObjectChanel);
        }
        else {
            if (Rooms[ObjectChanel]->CompleteLevel) {
                // �Ϸ� �� ������ ����
                Rooms[ObjectChanel]->SetRoomActive(false);
                DoTimer(Rooms[ObjectChanel]->GetUpdateTick(), &RoomManager::RemoveRoom, ObjectChanel);
            }
            else {
                // TODO �Ϸ�����ʾҴµ� ������� ó�� �����ð� ����ɾ�ΰ� �ð������� ����
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
	// �Ŵ����� ��ϵ� �������� ��ο��� �����͸� �Ѹ�.
	for (auto& room : Rooms)
	{
		room.second->Broadcast(sendBuffer);
	}
}
