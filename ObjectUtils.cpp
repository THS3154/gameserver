#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "Monster.h"
#include "Protocol.pb.h"
#include "MonsterTable.h"
#include "GameSession.h"
#include "CharacterDB.h"
#include "PlayerLevel.h"
atomic<int64> ObjectUtils::s_idGenerator = 1;
atomic<int64> ObjectUtils::s_idMonsterGenerator = 1;
atomic<int64> ObjectUtils::s_DBGenerator = 1;
PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session, int64 characterID)
{
	const int64 newid = s_idGenerator.fetch_add(1);
	PlayerRef player = make_shared<Player>();

	Protocol::ObjectInfo* objectinfo =  CharacterDB::SelectCharacterInfoToID(session->GetDBID(), characterID);
	if (objectinfo == nullptr)
	{
		// TODO 잘못된 접근 
		return nullptr;
	}
	
	session->myCharacterDBID[newid] = characterID;
    player->SetCharacterID(characterID);
	player->objectInfo->CopyFrom(*objectinfo);
	player->objectInfo->set_object_id(newid);
	player->objectInfo->set_leveltype(objectinfo->leveltype());
	player->objectInfo->set_creature_type(Protocol::CREATURE_TYPE_PLAYER);
	player->objectInfo->set_object_type(Protocol::OBJECT_TYPE_CREATURE);
	player->objectInfo->set_name(objectinfo->name());
	player->objectInfo->set_totalexp(PlayerLevel::GetTotalExp(player->objectInfo->playerlevel()));

	// 일단 기본스텟
    player->objectInfo->set_maxhp(player->GetAddLevelUpHP() * player->objectInfo->playerlevel());
	player->objectInfo->set_hp(500);
	player->objectInfo->set_mp(500);
	player->objectInfo->set_speed(300);
	player->objectInfo->set_shield(0);

	//위치값 저장
	player->posInfo = player->objectInfo->mutable_posinfo();
	player->posInfo->set_object_id(newid);
	player->session = session;

	std::lock_guard<std::mutex> lock(session->playerMutex);
	session->player = std::move(player);
	return session->player;
}


MonsterRef ObjectUtils::CreateMonster(int spawnerID, float x, float y, float z)
{
	const int64 newid = s_idGenerator.fetch_add(1);
	Protocol::MonsterID monsterID = Protocol::MONSTER_ID_GOBLIN;
	MonsterRef monster = make_shared<Monster>(spawnerID, monsterID);
	monster->objectInfo->set_object_id(newid);
	monster->objectInfo->set_leveltype(Protocol::LEVEL_TYPE_BASE);
	//monster->objectInfo->set_chanel(0);
	monster->objectInfo->set_monsterid(monsterID);
	monster->posInfo->set_object_id(newid);

	monster->objectInfo->set_creature_type(Protocol::CREATURE_TYPE_MONSTER);
	monster->objectInfo->set_object_type(Protocol::OBJECT_TYPE_CREATURE);
	
	monster->objectInfo->set_hp(MonsterTable::MonsterInfos[monsterID].max_hp);
	monster->objectInfo->set_mp(MonsterTable::MonsterInfos[monsterID].max_mp);
	monster->objectInfo->set_speed(MonsterTable::MonsterInfos[monsterID].defalut_speed);
	monster->objectInfo->set_shield(MonsterTable::MonsterInfos[monsterID].shield);
	monster->objectInfo->set_curexp(MonsterTable::MonsterInfos[monsterID].exp);			// 몬스터 경험치
	monster->objectInfo->set_gold(MonsterTable::MonsterInfos[monsterID].gold);			// 몬스터 골드
	monster->SetAggroRange(MonsterTable::MonsterInfos[monsterID].searchradius);			// 어그로

    for (auto drop : MonsterTable::MonsterInfos[monsterID].dropTable) {
        monster->DropTable.push_back(drop);
    }
    

	Protocol::PosInfo* posInfo = monster->objectInfo->mutable_posinfo();
	posInfo->set_x(x);
	posInfo->set_y(y);
	posInfo->set_z(z);


	return monster;
}
