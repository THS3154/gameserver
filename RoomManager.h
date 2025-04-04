#pragma once
#include "JobQueue.h"
class RoomManager : public JobQueue
{
public:
	
    RoomManager(int32 maxPlayer);
    RoomManager(int32 maxPlayer, bool _PrivateRoom);
	void JoinChanel(ObjectRef object, bool enter);
	void JoinChanel(uint32 chanel, ObjectRef object, bool enter);
	void LeaveChanel(PlayerRef player);


	void SetLevelType(Protocol::LevelType myLevelType){ MyLevelType = myLevelType;}
	void SetLevelName(string levelName){MyLevelName = levelName;}
	string GetLevelName(){return MyLevelName;}
	void SetMaxPlayer(int32 maxPlayer){MaxPlayer = maxPlayer;}
public:
	//��ü �޽���
	void Broadcast(SendBufferRef sendBuffer);
public:

    int GetRoomCnt(){return Rooms.size(); } // ä�� ����
    int GetJoinCnt(){return JoinCnt;}

	unordered_map<uint64, RoomRef> Rooms;

private:
    int JoinCnt = 0;        // �ش� �� �����ο�
	uint64 GetFirstAvailableID();
	RoomRef CreateChanel();
	void RemoveRoom(uint64 RoomID);

private:
	//�׽�Ʈ������ ��ä�ο� �Ѹ��� ����� ����
	int MaxPlayer = 2;
	Protocol::LevelType MyLevelType;
	string MyLevelName;
    bool PrivateRoom = false;
};

