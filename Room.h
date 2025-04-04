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
		Room 설정 및 기능
**************************************/
public:
	uint64 GetEnterPlayerCnt();			// 현재 접속중인 플레이어 수
	virtual void Init();				// 기본적인 룸 셋팅
	void SetRoomActive(bool bActive) {	// 룸 활성화 및 비활성화
		bRoomActive = bActive;
	}
	bool GetRoomActive(){return bRoomActive;}	// 현재 룸이 사용가능한 룸인지

	virtual void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);
	void MonsterBroadcast(SendBufferRef sendBuffer, MonsterRef monster);

	int32 updateTick = 33;				// 룸 업데이트 시간 기본값 0.1초
	virtual void UpdateTick();			// 룸을 업데이트 해줄때 호출됨.
	int32 GetUpdateTick() { return updateTick; }
	void SetUpdateTick(int Tick) { updateTick = Tick; }
	RoomRef GetRoomRef();

	// 플레어이 입퇴장 기능
	virtual bool EnterRoom(ObjectRef object);
	virtual bool LeaveRoom(ObjectRef object);
	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	bool AddObject(ObjectRef obejct);	// 룸에 오브젝트 추가(플레이어 입장때 사용)
	bool RemoveObject(uint64 objectid);	// 룸에 오브젝트 제거(플레이어 나갈때 사용)

    
    bool CompleteLevel = false;
    bool IsRoomActive(){return bRoomActive;}
    unordered_map<uint64, ObjectRef> GetObjects(){return _objects;}


    void PingOut();
protected:
	RoomRef MyRoom;						// 현재 룸 정보 자신이 들고있음
	bool bRoomActive = true;			// 룸 활성화 여부 기본값으로 활성화됨
	

	unordered_map<uint64, ObjectRef> _objects;		//플레이어 목록
	
	// 예약걸때 모든사람들에게 값 줄때
	uint64 NoneObjectID = 0;

    virtual void UpdateAI() override;
    
	
/**************************************
			플레이어 관련
**************************************/
protected:
	void PlayerAttack(PlayerRef player, Protocol::C_SKILL pkt);
	int GetGainExp(vector<uint64> objectIDs);		// 사냥 경험치 통합처리
	int GetGainGold(vector<uint64> objectIDs);		// 사냥 골드 통합관리
public:
	void HandleMove(Protocol::C_MOVE pkt);			// 플레이어 움직임 처리
	void HandleSkill(Protocol::C_SKILL pkt);		// 플레이어 공격 처리



};
