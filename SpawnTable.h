#pragma once
struct StructMonsterSpawn
{
    uint32 monsterID = -1;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};
class SpawnTable
{

public:
    static vector<StructMonsterSpawn> LoadSpawnMonstersFromXML(const char* FilePath);
};

