#pragma once

class PlayerLevel
{
public:
	PlayerLevel();
	~PlayerLevel();

	static void Init();
	static void AddExp(PlayerRef player, int gainedExp);
	static int GetTotalExp(int level){ return LevelExpTable[level]; }
private:
    static std::unordered_map<int, int> LevelExpTable;
	static int MaxLevel;

};

