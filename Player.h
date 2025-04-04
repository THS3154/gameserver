#pragma once
#include "Creature.h"
#include <unordered_map>
#include "PlayerInventory.h"
#include "PlayerEquiment.h"
enum class ESkillType {
    Melee,  // ���� ����
    Ranged, // ���Ÿ� ����
    AoE,    // ���� ���� (����)
    Cone    // ��ä�� ����
};
// ��ų ���� ����ü
struct FSkillData {
    std::string SkillName;
    ESkillType SkillType;
    float DamageRate;   // �⺻ ���ݷ¿��� ����
    float Range;		// ��ų ��� ���� �Ÿ�
    float Radius;		// ���� ���� �� �ݰ�
    float Angle;		// ��ä�� ���� �� ���� (Cone)
};




class Player : public Creature
{
public:
    Player();
    virtual ~Player();
    virtual ObjectType GetType() const override { return ObjectType::PLAYER; }



    
    float Hit(Protocol::SkillID skillid, bool isCritical);
    bool IsCritical();				// ũ��Ƽ�� Ȯ��
    void UseItem(Protocol::C_USEINVENTORY& pkt);
    FSkillData GetSkilIData(Protocol::SkillID skillid);

    PlayerInventory* Inventory;
    PlayerEquiment* Equipment;
    float GetDamage() { return Damage + Equipment->GetAttack(); }
    float GetDefense() { return Defense + Equipment->GetDefense(); }
    float GetCriticalChane() { return CriticalChance + Equipment->GetCriticalChance(); }
    float GetCriticalRate() { return CriticalRate * (Equipment->GetCriticalRate() <= 0 ? Equipment->GetCriticalRate() : 1); }
    int GetAddLevelUpHP(){return AddLevelUpHP;}
    int GetPlayerMaxHp(){ return AddLevelUpHP * objectInfo->playerlevel() + Equipment->GetHp();}

    void SetCharacterID(uint64 ID) { CharacterID  = ID;}
    uint64 GetCharacterID(){return CharacterID;}
private:
    std::unordered_map<int, FSkillData> SkillDatabase = {
        { Protocol::SKILL_BASE_DEFAULT_ATTACK, {"Default Attack", ESkillType::Melee, 1.3f, 200.0f, 360.0f, 0.0f} },   // ���� ����
    };
    float Damage = 100.f;
    float Defense = 0.f;
    float CriticalChance = 0.5f;	// 50��
    float CriticalRate = 2.5f;		// ũ��Ƽ�ö߸� 2.5��

    int AddLevelUpHP = 500;
    uint64 CharacterID = 0;
public:
    //weak_ptr<GameSession> session;
};

