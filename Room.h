#pragma once
#include "JobQueue.h"
#include "MonsterInfo.h"
#include "PlayerUtil.h"
#include "MonsterUtil.h"
#include <unordered_map>



class Room : public JobQueue, public PlayerUtil, public MonsterUtil
{
public:
	Room();
	virtual ~Room();

/**************************************
		Room ���� �� ���
**************************************/
public:
	uint64 GetEnterPlayerCnt();			// ���� �������� �÷��̾� ��
	virtual void Init();				// �⺻���� �� ����
	void SetRoomActive(bool bActive) {	// �� Ȱ��ȭ �� ��Ȱ��ȭ
		bRoomActive = bActive;
	}
	bool GetRoomActive(){return bRoomActive;}	// ���� ���� ��밡���� ������

	virtual void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);
	void MonsterBroadcast(SendBufferRef sendBuffer, MonsterRef monster);

	int32 updateTick = 33;				// �� ������Ʈ �ð� �⺻�� 0.1��
	virtual void UpdateTick();			// ���� ������Ʈ ���ٶ� ȣ���.
	int32 GetUpdateTick() { return updateTick; }
	void SetUpdateTick(int Tick) { updateTick = Tick; }
	RoomRef GetRoomRef();

	// �÷����� ������ ���
	virtual bool EnterRoom(ObjectRef object);
	virtual bool LeaveRoom(ObjectRef object);
	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	bool AddObject(ObjectRef obejct);	// �뿡 ������Ʈ �߰�(�÷��̾� ���嶧 ���)
	bool RemoveObject(uint64 objectid);	// �뿡 ������Ʈ ����(�÷��̾� ������ ���)

    
    bool CompleteLevel = false;
    bool IsRoomActive(){return bRoomActive;}
    unordered_map<uint64, ObjectRef> GetObjects(){return _objects;}


    void PingOut();
protected:
	RoomRef MyRoom;						// ���� �� ���� �ڽ��� �������
	bool bRoomActive = true;			// �� Ȱ��ȭ ���� �⺻������ Ȱ��ȭ��
	

	unordered_map<uint64, ObjectRef> _objects;		//�÷��̾� ���
	
	// ����ɶ� ������鿡�� �� �ٶ�
	uint64 NoneObjectID = 0;

    virtual void UpdateAI() override;
    
	
/**************************************
			�÷��̾� ����
**************************************/
protected:
	void PlayerAttack(PlayerRef player, Protocol::C_SKILL pkt);
	int GetGainExp(vector<uint64> objectIDs);		// ��� ����ġ ����ó��
	int GetGainGold(vector<uint64> objectIDs);		// ��� ��� ���հ���
public:
	void HandleMove(Protocol::C_MOVE pkt);			// �÷��̾� ������ ó��
	void HandleSkill(Protocol::C_SKILL pkt);		// �÷��̾� ���� ó��



};
