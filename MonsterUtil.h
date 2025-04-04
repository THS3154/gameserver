#pragma once
#include "PublicUtil.h"
#include "MonsterInfo.h"
/*******************************************
			���� ���
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


    // �ش� ���� ���� �����ʰ� ���̺��߿� ����X
    unordered_map<uint64, uint64> _levelMonsterSpawn;
    unordered_map<uint64, SpawnLocation> _levelMonsterSpawnLocation;

    size_t _monsterProcessIndex = 0; // ó���� ���� �ε���
    const size_t MONSTER_BATCH_SIZE = 30; // �� ���� ó���� ���� ��
    

/**************************************
            ���� ����
**************************************/
protected:
    
    unordered_map<uint64, MonsterRef> _monsters;	//���� ���

    virtual bool IsMonsterDie(uint64 objectid);				// ���� ���üũ
    void SetMonsterSpawner(uint64 spawnerID, uint64 monsterType, float x, float y, float z);	// ������ ��ġ ����
    void MonsterSpawn(vector<uint64> spawnerIDs);	// ���� ����
    void MonsterSpawn(uint64 spawnerID);			// ���� ���� ����
    void MonsterHit(uint64 objectID, float Damage);	// ���� ��Ʈ ó��
    Protocol::PosInfo* SendMonsterMove(MonsterRef monster);		// ���� ������ ó��
    void MonsterDeSpawn(vector<uint64> objectIDs);	// ���� ����
    virtual bool DeSpawnMonster(uint64 objectid);

    
};

