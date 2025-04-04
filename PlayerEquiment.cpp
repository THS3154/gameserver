#include "pch.h"
#include "PlayerEquiment.h"
#include "ItemUtils.h"
#include "CharacterDB.h"

PlayerEquiment::PlayerEquiment()
{
    
    equipments["head"] = Protocol::ItemData();
    equipments["weapon"] = Protocol::ItemData();


}

void PlayerEquiment::PrintEquiments()
{
    if (equipments["weapon"].code() != 0) {
        ServerItemData* item = ItemUtils::GetItem(equipments["weapon"].code());
        LOG("Weapon : " << item->Name << " Attack : " << item->Attack);
    }
    else { LOG("Weapon : Not Equ"); }

    if (equipments["head"].code() != 0) {
        ServerItemData* item = ItemUtils::GetItem(equipments["head"].code());
        LOG("Head : " << item->Name << " Defense : " << item->Defense);
    }
    else { LOG("Not Equ"); }
}

bool PlayerEquiment::SetEquimentItem(string slotName, uint64 instanceID, int code)
{   
    if(!IsEquiment(slotName)) return false;

    if (code == 0) {
        equipments[slotName].set_code(0);
        return false;
    }
    ServerItemData* item = ItemUtils::GetItem(code);
    if (item == nullptr) {
        LOG("�߸��� ����������");
        return false;
    }

    if (equipments[slotName].code() != 0) {
        // TODO ���� ���� �ɼ� ����
        ServerItemData* item = ItemUtils::GetItem(equipments[slotName].code());
        AddAttack           -= item->Attack;
        AddDefense          -= item->Defense;
        AddCriticalRate     -= item->CriticalRate;
        AddCriticalChance   -= item->Critical;
        AddHP               -= item->Hp;
    }


    if (equipments[slotName].id() == instanceID) {
        // ���� ��� ���� ����
        equipments[slotName].set_code(0);
        equipments[slotName].set_id(0);
        return true;
    }
    
    //�������
    equipments[slotName].set_code(code);
    equipments[slotName].set_id(instanceID);
    
    AddAttack           += item->Attack;
    AddDefense          += item->Defense;
    AddCriticalRate     += item->CriticalRate;
    AddCriticalChance   += item->Critical;
    AddHP               += item->Hp;
    return true;

}

void PlayerEquiment::LoadEquipment(Protocol::S_DB_LOADEQUIPMENT& pkt)
{
    for (auto item : pkt.items()) {
        SetEquimentItem(item.slot_name(),item.id(), item.code());
    }
    
    bLoad = true;
}

vector<Protocol::ItemData> PlayerEquiment::GetEquipments()
{
    vector<Protocol::ItemData> items;
    for (auto& item : equipments) {
        if (item.second.id() != 0) {
            items.push_back(item.second);
        }
    }
    return items;
}

bool PlayerEquiment::IsEquiment(string slotName)
{
    if (equipments.find(slotName) == equipments.end()) {
        LOG("�߸��� ��������");
        return false;
    }
    else {
        return true;
    }

    
}

void PlayerEquiment::DBSendEquipment(uint64 characterID)
{
    Protocol::C_DB_EQUIPMENT pkt;
    pkt.set_characterid(characterID);
    for (auto equ : equipments) {
        Protocol::Equipment* item = pkt.add_items();
        item->set_character_id(characterID);
        item->set_code(equ.second.id());
        item->set_slot_name(equ.first);
    }

    CharacterDB::UpdatePlayerEquipment(pkt);
}

Protocol::S_LOADEQUIPMENT PlayerEquiment::ClientSendEquipmentPkt()
{
    Protocol::S_LOADEQUIPMENT pkt;
    for (auto equ : equipments) {
        Protocol::ItemData* item = pkt.add_items();
        item->CopyFrom(equ.second);
    }
    return pkt;
}
