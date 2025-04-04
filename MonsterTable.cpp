#include "pch.h"
#include "MonsterTable.h"
#include "tinyxml2.h"
using namespace tinyxml2;
unordered_map<uint64, StatInfo> MonsterTable::MonsterInfos;

bool MonsterTable::LoadMonsterInfo()
{
	// �������� xml���� �޾ƿ�
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
                MonsterInfo.max_hp = atoi(Hp->GetText());                              //HP ����
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
                MonsterInfo.shield = atoi(Shield->GetText());                          // ����


            if (XMLElement* SearchRadius = item->FirstChildElement("SearchRadius"))
                MonsterInfo.searchradius = atoi(SearchRadius->GetText());              // ��׷� ����

            if (XMLElement* RegenTime = item->FirstChildElement("RegenTime"))
                MonsterInfo.regentime = atoi(RegenTime->GetText());                    // ���� �����ֱ�

            if (XMLElement* Exp = item->FirstChildElement("Exp"))
                MonsterInfo.exp = std::stof(Exp->GetText());                           // ����ġ

            if (XMLElement* Gold = item->FirstChildElement("Gold"))
                MonsterInfo.gold = std::stof(Gold->GetText());                          // ����ġ


            // ���� ������� ��� ����

            if (XMLElement* dropTable = item->FirstChildElement("DropTable"))
            {
                for (XMLElement* drop = dropTable->FirstChildElement("Drop"); drop != nullptr; drop = drop->NextSiblingElement("Drop"))
                {
                    int code = 0;
                    float rate = 0.0f;

                    drop->QueryIntAttribute("code", &code);
                    drop->QueryFloatAttribute("rate", &rate);

                    // ��: ����ü�� ����
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
    //�׽�Ʈ������ Ȯ���ϱ����� ���� ������ ��ϵ� ���������� �ҷ���.
    for (auto monster : MonsterInfos) {
        StatInfo monsterInfo = monster.second;
        LOG("MonsterID : " << monster.first << " MonsterName : " << monsterInfo.name << " MonsterHP : " << monsterInfo.max_hp);
    }
    
}
