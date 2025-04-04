#include "pch.h"
#include "MonsterTable.h"
#include "tinyxml2.h"
using namespace tinyxml2;
unordered_map<uint64, StatInfo> MonsterTable::MonsterInfos;

bool MonsterTable::LoadMonsterInfo()
{
	// 몬스터정보 xml에서 받아옴
    return LoadMonstersFromXML("MonsterInfo.xml");
}

bool MonsterTable::LoadMonstersFromXML(const char* FilePath)
{
    XMLDocument doc;
    if (doc.LoadFile(FilePath) == XML_SUCCESS)
    {
        XMLElement* root = doc.FirstChildElement("Monsters");
        for (XMLElement* item = root->FirstChildElement("Monster"); item != nullptr; item = item->NextSiblingElement("Monster"))
        {
            int id = item->IntAttribute("id");
            StatInfo MonsterInfo;
            MonsterInfo.name = item->FirstChildElement("Name")->GetText();

            if (XMLElement* Hp = item->FirstChildElement("Hp")) {
                MonsterInfo.max_hp = atoi(Hp->GetText());                              //HP 저장
                MonsterInfo.cur_hp = atoi(Hp->GetText());
            }
            if (XMLElement* Mp = item->FirstChildElement("Mp")) {
                MonsterInfo.max_mp = atoi(Mp->GetText());                   // Mp
                MonsterInfo.cur_mp = atoi(Mp->GetText());

            }

            if (XMLElement* Speed = item->FirstChildElement("Speed")) {
                MonsterInfo.defalut_speed = atoi(Speed->GetText());                   // Mp
                MonsterInfo.cur_speed = atoi(Speed->GetText());

            }

            if (XMLElement* Shield = item->FirstChildElement("Shield"))
                MonsterInfo.shield = atoi(Shield->GetText());                          // 쉴드


            if (XMLElement* SearchRadius = item->FirstChildElement("SearchRadius"))
                MonsterInfo.searchradius = atoi(SearchRadius->GetText());              // 어그로 범위

            if (XMLElement* RegenTime = item->FirstChildElement("RegenTime"))
                MonsterInfo.regentime = atoi(RegenTime->GetText());                    // 몬스터 리젠주기

            if (XMLElement* Exp = item->FirstChildElement("Exp"))
                MonsterInfo.exp = std::stof(Exp->GetText());                           // 경험치

            if (XMLElement* Gold = item->FirstChildElement("Gold"))
                MonsterInfo.gold = std::stof(Gold->GetText());                          // 경험치


            // 몬스터 잡았을때 드랍 여부

            if (XMLElement* dropTable = item->FirstChildElement("DropTable"))
            {
                for (XMLElement* drop = dropTable->FirstChildElement("Drop"); drop != nullptr; drop = drop->NextSiblingElement("Drop"))
                {
                    int code = 0;
                    float rate = 0.0f;

                    drop->QueryIntAttribute("code", &code);
                    drop->QueryFloatAttribute("rate", &rate);

                    // 예: 구조체로 보관
                    MonsterDropInfo dropInfo;
                    dropInfo.code = code;
                    dropInfo.rate = rate;

                    MonsterInfo.dropTable.push_back(dropInfo);
                }
            }

            MonsterInfos.insert({ id, MonsterInfo });
        }
        LOG("Monster Load Success!!!");
        return true;
    }
    else {
        LOG("Monster Load Fail!!!");
        return false;
    }
}

void MonsterTable::PrintMonsters()
{
    //테스트용으로 확인하기위해 현재 서버에 등록된 몬스터정보를 불러옴.
    for (auto monster : MonsterInfos) {
        StatInfo monsterInfo = monster.second;
        LOG("MonsterID : " << monster.first << " MonsterName : " << monsterInfo.name << " MonsterHP : " << monsterInfo.max_hp);
    }
    
}
