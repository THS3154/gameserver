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
    bool GetLoadEquipment(){ return bLoad;}                                 // 장비를 불러왔는지?
    int GetAttack(){ return AddAttack;}
    int GetDefense(){ return AddDefense;}
    int GetSpeed() { return AddSpeed; }
    float GetCriticalRate() { return AddCriticalRate; }
    float GetCriticalChance() { return AddCriticalChance; }
    int GetHp(){return AddHP;}
    void PrintEquiments();                              // 장착중인 장비 확인
    bool SetEquimentItem(string slotName, uint64 instanceID, int code); // 장비 장착 및 해제
    void LoadEquipment(Protocol::S_DB_LOADEQUIPMENT& pkt);  // 첫 장비 로드
    vector<Protocol::ItemData> GetEquipments();         // 장착중인 장비 목록
    bool IsEquiment(string slotName);                   // 장비 장착여부
    void DBSendEquipment(uint64 characterID);           // DB서버에 저장
    Protocol::S_LOADEQUIPMENT ClientSendEquipmentPkt(); // 현재 장착중인 장비데이터를 패킷화
private:
    unordered_map<string, Protocol::ItemData> equipments;
    int AddAttack = 0;                          // 장착으로 증가한 공격력
    int AddDefense = 0;                         // 장착으로 증가한 방어력
    int AddSpeed = 0;                           // 장착으로 증가한 스피드
    int AddHP = 0;                              // 장착으로 증가한 체력
    float AddCriticalRate = 0.0f;               // 장착으로 증가한 치피증 
    float AddCriticalChance = 0.0f;             // 장착으로 증가한 크리티컬확률


    bool bLoad = false;
};


