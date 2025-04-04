#include "pch.h"
#include "ItemUtils.h"
#include "tinyxml2.h"
#include <unordered_map>
#include <random>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist(0.0f, 1.0f);


using namespace tinyxml2;
std::unordered_map<int, ServerItemData> ItemUtils::ItemDB;

bool ItemUtils::LoadItemsFromXML(const char* FilePath)
{
    XMLDocument doc;
    if (doc.LoadFile(FilePath) == XML_SUCCESS)
    {
        XMLElement* root = doc.FirstChildElement("Items");
        for (XMLElement* item = root->FirstChildElement("Item"); item != nullptr; item = item->NextSiblingElement("Item"))
        {
            int id = item->IntAttribute("id");
            ServerItemData data;
            data.code = id;
            data.Name = item->FirstChildElement("Name")->GetText();
            data.Type = GetItemTypeFromString(item->FirstChildElement("Type")->GetText());

            if (XMLElement* atk = item->FirstChildElement("Attack"))
                data.Attack = atoi(atk->GetText());                             //공격력
            if (XMLElement* atk = item->FirstChildElement("Critical"))
                data.Critical = (atoi(atk->GetText()) / 100.0f);                // 크리확률
            if (XMLElement* atk = item->FirstChildElement("CriticalRate"))
                data.CriticalRate = 1.0 + (atoi(atk->GetText()) / 100.0f);      // 치피증

            if (XMLElement* Slot = item->FirstChildElement("Slot"))
                data.slot = Slot->GetText();                                    // 슬롯명
            
            if (XMLElement* atk = item->FirstChildElement("Defense"))
                data.Defense = atoi(atk->GetText());                            // 방어력

            if (XMLElement* Hp = item->FirstChildElement("Hp"))
                data.Hp = atoi(Hp->GetText());

            if (XMLElement* heal = item->FirstChildElement("Heal"))
                data.Heal = atoi(heal->GetText());                              // 회복량

            if (XMLElement* drop = item->FirstChildElement("Drop"))
                data.Drop = std::stof(drop->GetText());                         // 드럅률
            

            ItemDB[id] = data;
        }
        
        LOG("Item Load Success!!!");
        return true;
    }
    else {
        LOG("Item Load Fail!!!");
        return false;
    }
}


ServerItemData* ItemUtils::GetItem(int ItemCode)
{
    if (ItemDB.find(ItemCode) == ItemDB.end())
    {
        return nullptr;
    }
    else {
        return &ItemDB[ItemCode];
    }
}

float ItemUtils::RandomFloat()
{
    return dist(gen);
}
