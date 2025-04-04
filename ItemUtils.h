#pragma once

enum ItemType {
    Unknown,
    Consumable,  // �Ҹ�ǰ (���� ��)
    Equipment,   // ��� (����, ��)
    Quest,       // ����Ʈ ������
    Material     // ���� ���
};

struct ServerItemData
{
    std::string Name;
    string slot;
    ItemType Type;
    int code = 0;
    int Attack = 0;
    int Heal = 0;
    int Hp = 0;
    float Critical = 0;
    float CriticalRate = 0;
    int Defense = 0;
    float Drop = 0.0f;
};


class ItemUtils
{

private:
    static ItemType GetItemTypeFromString(const std::string& typeStr)
    {
        if (typeStr == "Consumable") return ItemType::Consumable;
        if (typeStr == "Equipment")  return ItemType::Equipment;
        if (typeStr == "Quest")      return ItemType::Quest;
        if (typeStr == "Material")   return ItemType::Material;
        return ItemType::Unknown;
    }

public:
    static bool LoadItemsFromXML(const char* FilePath);
    static ServerItemData* GetItem(int ItemCode);

    static float RandomFloat();                             // ����Ҷ� ����ó��
public:
    static std::unordered_map<int, ServerItemData> ItemDB;
};

