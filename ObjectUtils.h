#pragma once
class ObjectUtils
{

public:
	static PlayerRef CreatePlayer(GameSessionRef session, int64 characterID);
	static MonsterRef CreateMonster(int spawnerID, float x, float y, float z);
    static atomic<int64> s_DBGenerator;

private:
	static atomic<int64> s_idGenerator;
	static atomic<int64> s_idMonsterGenerator;
};

