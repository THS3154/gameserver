#include "pch.h"
#include "SpawnTable.h"
#include "tinyxml2.h"
using namespace tinyxml2;

vector<StructMonsterSpawn> SpawnTable::LoadSpawnMonstersFromXML(const char* FilePath)
{
    vector<StructMonsterSpawn> list;
    XMLDocument doc;
    if (doc.LoadFile(FilePath) == XML_SUCCESS)
    {
        XMLElement* root = doc.FirstChildElement("Monsters");
        for (XMLElement* item = root->FirstChildElement("Monster"); item != nullptr; item = item->NextSiblingElement("Monster"))
        {
            StructMonsterSpawn MonsterInfo;
            MonsterInfo.monsterID = item->IntAttribute("monsterid");
            MonsterInfo.x = item->FloatAttribute("x");
            MonsterInfo.y = item->FloatAttribute("y");
            MonsterInfo.z = item->FloatAttribute("z");

            list.push_back(MonsterInfo);
        }
        //LOG("MonsterSpawn Load Success!!!");
    }
    else {
        LOG("MonsterSpawn Load Fail!!!");
    }

    return list;

}
