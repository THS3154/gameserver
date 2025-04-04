#pragma once
#include "PublicUtil.h"
#include "MonsterInfo.h"
/*******************************************
			몬스터 기능
*******************************************/
class MonsterUtil : virtual public PublicUtil
{
public:
	MonsterUtil();
	virtual ~MonsterUtil();


    virtual void UpdateAI();

    bool StopMonsterSpawn = false;
    int GetMonsterTickTime(){return MonstertTickTime;}
private:
    int MonstertTickTime = 100;
protected:
    RoomRef MonsterUtilRoom = nullptr;
	std::vector<uint64> GetNearbyPlayers(float monsterX, float monsterY, int searchRange);

	void AddMonsterToGrid(uint64 monsterID, float x, float y);
	void RemoveMonsterToGrid(uint64 monsterID, float x, float y);


    // 해당 레벨 고유 스포너값 라이브중에 수정X
    unordered_map<uint64, uint64> _levelMonsterSpawn;
    unordered_map<uint64, SpawnLocation> _levelMonsterSpawnLocation;

    size_t _monsterProcessIndex = 0; // 처리할 몬스터 인덱스
    const size_t MONSTER_BATCH_SIZE = 30; // 한 번에 처리할 몬스터 수
    

/**************************************
            몬스터 관련
**************************************/
protected:
    
    unordered_map<uint64, MonsterRef> _monsters;	//몬스터 목록

    virtual bool IsMonsterDie(uint64 objectid);				// 몬스터 사망체크
    void SetMonsterSpawner(uint64 spawnerID, uint64 monsterType, float x, float y, float z);	// 스포너 위치 설정
    void MonsterSpawn(vector<uint64> spawnerIDs);	// 몬스터 스폰
    void MonsterSpawn(uint64 spawnerID);			// 몬스터 스폰 메인
    void MonsterHit(uint64 objectID, float Damage);	// 몬스터 히트 처리
    Protocol::PosInfo* SendMonsterMove(MonsterRef monster);		// 몬스터 움직임 처리
    void MonsterDeSpawn(vector<uint64> objectIDs);	// 몬스터 디스폰
    virtual bool DeSpawnMonster(uint64 objectid);

    
};

