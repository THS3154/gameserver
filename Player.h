#pragma once
#include "Creature.h"
#include <unordered_map>
#include "PlayerInventory.h"
#include "PlayerEquiment.h"
enum class ESkillType {
    Melee,  // 근접 공격
    Ranged, // 원거리 공격
    AoE,    // 범위 공격 (원형)
    Cone    // 부채꼴 공격
};
// 스킬 정보 구조체
struct FSkillData {
    std::string SkillName;
    ESkillType SkillType;
    float DamageRate;   // 기본 공격력에서 비중
    float Range;		// 스킬 사용 가능 거리
    float Radius;		// 범위 공격 시 반경
    float Angle;		// 부채꼴 공격 시 각도 (Cone)
};




class Player : public Creature
{
public:
    Player();
    virtual ~Player();
    virtual ObjectType GetType() const override { return ObjectType::PLAYER; }



    
    float Hit(Protocol::SkillID skillid, bool isCritical);
    bool IsCritical();				// 크리티컬 확인
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
        { Protocol::SKILL_BASE_DEFAULT_ATTACK, {"Default Attack", ESkillType::Melee, 1.3f, 200.0f, 360.0f, 0.0f} },   // 근접 공격
    };
    float Damage = 100.f;
    float Defense = 0.f;
    float CriticalChance = 0.5f;	// 50퍼
    float CriticalRate = 2.5f;		// 크리티컬뜨면 2.5배

    int AddLevelUpHP = 500;
    uint64 CharacterID = 0;
public:
    //weak_ptr<GameSession> session;
};

