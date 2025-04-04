#include "pch.h"
#include "Monster.h"
#include "Protocol.pb.h"
#include "MonsterTable.h"
#include "Utils.h"


Monster::Monster()
{
}

Monster::Monster(int spawnerid, Protocol::MonsterID monsterID)
	: SpawnerID(spawnerid), MonsterID(monsterID)
{
	
	//TODO ���߿� ���� ���̺��� �����ͼ� ����ġ �̸� ��� ����
	stat.max_hp			= MonsterTable::MonsterInfos[monsterID].max_hp;
	stat.max_mp			= MonsterTable::MonsterInfos[monsterID].max_mp;
	stat.defalut_speed	= MonsterTable::MonsterInfos[monsterID].defalut_speed;
	stat.cur_hp			= MonsterTable::MonsterInfos[monsterID].max_hp;
	stat.cur_mp			= MonsterTable::MonsterInfos[monsterID].max_mp;
	stat.defalut_speed	= MonsterTable::MonsterInfos[monsterID].defalut_speed;
	stat.shield			= MonsterTable::MonsterInfos[monsterID].shield;
	stat.cur_speed		= MonsterTable::MonsterInfos[monsterID].searchradius;
	stat.regentime		= MonsterTable::MonsterInfos[monsterID].regentime;
}

Monster::~Monster()
{
}

bool Monster::OnHit(float damage)
{
	//���差���� ���� �������� �Ծ����� ���差 ������ ������
	float haveDamage = damage - stat.shield;	

	// ���������� ���尡 ���� �������� ó����
	stat.shield = stat.shield - damage;
    
	if(haveDamage <= 0){
        objectInfo->set_shield(stat.shield);
        return false;
    }
    else {
        objectInfo->set_shield(0);
    }

	// ���ݹ޾Ƽ� �������� ����.
	stat.cur_hp = stat.cur_hp - haveDamage;
    objectInfo->set_hp(stat.cur_hp);
	if (stat.cur_hp <= 0) {
		return true;
	}
	return false;
}

bool Monster::Heel(float heel)
{
	stat.cur_hp = stat.cur_hp + heel;
	if(stat.cur_hp > stat.max_hp)
		stat.cur_hp = stat.max_hp;
	return true;
}

void Monster::Die()
{
	
}

void Monster::SetDestPos(MonsterRef monster, unordered_map<uint64, SpawnLocation>  _levelMonsterSpawnLocation)
{
	// ������ ����

	uint64 spawnerID = monster->SpawnerID;
	monster->bmovePos = true;	// ������ �ִٰ� ����
	monster->movePosInfo->set_x(_levelMonsterSpawnLocation[spawnerID].x + Utils::GetRandomFloat(aggroRange));
	monster->movePosInfo->set_y(_levelMonsterSpawnLocation[spawnerID].y + Utils::GetRandomFloat(aggroRange));
	monster->movePosInfo->set_z(monster->objectInfo->posinfo().z());
	monster->SetState(MonsterState::MOVE);
}

void Monster::SetDestPos(MonsterRef monster, ObjectRef object)
{
	if (object == nullptr) {
		// Ÿ���� null�ϰ��
		monster->LostTarget();
		return;
	}
	
	monster->bmovePos = true;	// ������ �ִٰ� ����
	monster->movePosInfo->set_x(object->posInfo->x());
	monster->movePosInfo->set_y(object->posInfo->y());
	monster->movePosInfo->set_z(object->posInfo->z());
	monster->SetState(MonsterState::AGGRO);
}


void Monster::VoidServerMove(MonsterRef monster, float deltaTime)
{
    uint64 spawnerID = monster->SpawnerID;

    Vector3 NowPos(monster->objectInfo->posinfo().x(), monster->objectInfo->posinfo().y(), monster->objectInfo->posinfo().z());
    Vector3 DestPos(monster->movePosInfo->x(), monster->movePosInfo->y(), monster->movePosInfo->z());
    Vector3 Direction = (DestPos - NowPos).GetSafeNormal();
    Vector3 NewPosition = NowPos + (Direction * monster->objectInfo->speed() * deltaTime);


    //������ �Ȱ��� �̵��� ��ǥ���� ��������.
    monster->posInfo->set_x(NewPosition.X);
    monster->posInfo->set_y(NewPosition.Y);
    monster->posInfo->set_movestate(Protocol::MOVE_STATE_RUN);

    // �ش���ġ ����
    float Dist = monster->Distance2D(DestPos.X, DestPos.Y);
    if (Dist <= 30.0f)
    {
        monster->posInfo->set_x(NewPosition.X);
        monster->posInfo->set_y(NewPosition.Y);


        if (monster->GetState() == MonsterState::MOVE) {
            monster->posInfo->set_movestate(Protocol::MOVE_STATE_IDLE);
            monster->SetState(MonsterState::IDLE);
            monster->bmovePos = false;; // �̵� ����
        }

    }

}


Protocol::PosInfo* Monster::ServerMove(MonsterRef monster, float deltaTime)
{
	uint64 spawnerID = monster->SpawnerID;
	
	Vector3 NowPos(monster->objectInfo->posinfo().x(), monster->objectInfo->posinfo().y(), monster->objectInfo->posinfo().z());
	Vector3 DestPos(monster->movePosInfo->x(), monster->movePosInfo->y(), monster->movePosInfo->z());
	Vector3 Direction = (DestPos - NowPos).GetSafeNormal();
	Vector3 NewPosition = NowPos + (Direction * monster->objectInfo->speed() * deltaTime);
	
	
	Protocol::PosInfo* pos = new Protocol::PosInfo();
	pos->CopyFrom(monster->objectInfo->posinfo());
	pos->set_x(NewPosition.X);
	pos->set_y(NewPosition.Y);
	pos->set_movestate(Protocol::MOVE_STATE_RUN);

	//������ �Ȱ��� �̵��� ��ǥ���� ��������.
	monster->posInfo->set_x(NewPosition.X);
	monster->posInfo->set_y(NewPosition.Y);
	monster->posInfo->set_movestate(Protocol::MOVE_STATE_RUN);
	
	// �ش���ġ ����
	float Dist = monster->Distance2D(DestPos.X, DestPos.Y);
	if (Dist <= 30.0f)
	{
		pos->set_x(DestPos.X);
		pos->set_y(DestPos.Y);
		monster->posInfo->set_x(NewPosition.X);
		monster->posInfo->set_y(NewPosition.Y);
		
		
		if (monster->GetState() == MonsterState::MOVE) {
			monster->posInfo->set_movestate(Protocol::MOVE_STATE_IDLE);
			pos->set_movestate(Protocol::MOVE_STATE_IDLE);
			monster->SetState(MonsterState::IDLE);
			monster->bmovePos = false;; // �̵� ����
		}
		
	}

	//SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	
	return pos;
}

Protocol::PosInfo* Monster::ServerMove(MonsterRef monster, ObjectRef object, float deltaTime)
{
	uint64 spawnerID = monster->SpawnerID;
	if(!object) return nullptr;

    Protocol::PosInfo* pos = new Protocol::PosInfo();
	
	pos->CopyFrom(monster->objectInfo->posinfo());
	pos->set_x(object->posInfo->x());
	pos->set_y(object->posInfo->y());
	pos->set_movestate(Protocol::MOVE_STATE_RUN);
	//������ �Ȱ��� �̵��� ��ǥ���� ��������.
	Protocol::PosInfo* mypos = monster->objectInfo->mutable_posinfo();
	mypos->set_x(pos->x());
	mypos->set_y(pos->y());
	mypos->set_movestate(Protocol::MOVE_STATE_RUN);

	return pos;
}

