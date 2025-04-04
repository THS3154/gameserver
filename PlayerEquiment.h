#pragma once

struct EquCodes {
    int32 code = 0;
    uint64 instanceID = 0;
};
class PlayerEquiment
{
public:
    PlayerEquiment();

public:
    bool GetLoadEquipment(){ return bLoad;}                                 // ��� �ҷ��Դ���?
    int GetAttack(){ return AddAttack;}
    int GetDefense(){ return AddDefense;}
    int GetSpeed() { return AddSpeed; }
    float GetCriticalRate() { return AddCriticalRate; }
    float GetCriticalChance() { return AddCriticalChance; }
    int GetHp(){return AddHP;}
    void PrintEquiments();                              // �������� ��� Ȯ��
    bool SetEquimentItem(string slotName, uint64 instanceID, int code); // ��� ���� �� ����
    void LoadEquipment(Protocol::S_DB_LOADEQUIPMENT& pkt);  // ù ��� �ε�
    vector<Protocol::ItemData> GetEquipments();         // �������� ��� ���
    bool IsEquiment(string slotName);                   // ��� ��������
    void DBSendEquipment(uint64 characterID);           // DB������ ����
    Protocol::S_LOADEQUIPMENT ClientSendEquipmentPkt(); // ���� �������� ������͸� ��Ŷȭ
private:
    unordered_map<string, Protocol::ItemData> equipments;
    int AddAttack = 0;                          // �������� ������ ���ݷ�
    int AddDefense = 0;                         // �������� ������ ����
    int AddSpeed = 0;                           // �������� ������ ���ǵ�
    int AddHP = 0;                              // �������� ������ ü��
    float AddCriticalRate = 0.0f;               // �������� ������ ġ���� 
    float AddCriticalChance = 0.0f;             // �������� ������ ũ��Ƽ��Ȯ��


    bool bLoad = false;
};


