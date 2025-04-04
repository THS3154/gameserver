#pragma once
#include <map>
#include <unordered_map>
using std::unordered_map;
#include "MonsterInfo.h"
class MonsterTable
{
public:
	static bool LoadMonsterInfo();
    static bool LoadMonstersFromXML(const char* FilePath);

    static void PrintMonsters();
public:
	static unordered_map<uint64, StatInfo> MonsterInfos;
};
