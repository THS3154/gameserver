#pragma once
#include "Creature.h"

enum MonsterState
{
	IDLE = 0,
	MOVE = 1,
	ATTACK = 2,
	AGGRO = 3
};

class Monster : public Creature
{
public:
	Monster();
	Monster(int spawnerid, Protocol::MonsterID monsterID);
	~Monster();

	
	virtual ObjectType GetType() const override { return ObjectType::MONSTER; }

	void SetTarget(uint64 _target) { 
		target = _target; 
		SetState(MonsterState::AGGRO);
	}

	void SetState(MonsterState _state) { state = _state; }
	void SetAggroRange(float _aggroRange) { aggroRange = _aggroRange; }
	uint64 GetTarget(){return target;}
	float GetAggroRange(){return aggroRange;}
	MonsterState GetState(){return state;}
	void LostTarget(){
		SetState(MonsterState::IDLE);
		bmovePos = false;
		target = 0;
	}
public:
	Protocol::MonsterID MonsterID;
	int SpawnerID;		// ������ �ش簪���� �ٽ� ������û�ϸ� ������ ���ͷ� ������

	// ���� ����
private:
	MonsterState state = MonsterState::IDLE;
	uint64 target = 0;
	float aggroRange = 500.f;
public:
    bool Hit();
	bool OnHit(float damage);		// ü���� 0���Ϸ� �������� true
	bool Heel(float heel);
	void Die();

    vector<MonsterDropInfo> DropTable;
public:
	void SetDestPos(MonsterRef monster, unordered_map<uint64, SpawnLocation>  _levelMonsterSpawnLocation);
	void SetDestPos(MonsterRef monster, ObjectRef object);
    void VoidServerMove(MonsterRef monster, float deltaTime);
    Protocol::PosInfo* ServerMove(MonsterRef monster, float deltaTime);
    Protocol::PosInfo* ServerMove(MonsterRef monster, ObjectRef object, float deltaTime);
};

