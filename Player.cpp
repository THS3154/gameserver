#include "pch.h"
#include <algorithm>
#include "Player.h"
#include "ItemUtils.h"
#include "Room.h"
Player::Player()
{
	_isPlayer = true;

    Inventory = new PlayerInventory();
    Equipment = new PlayerEquiment();
}

Player::~Player()
{

}

float Player::Hit(Protocol::SkillID skillid, bool isCritical)
{
    
	float DamageRate = SkillDatabase[skillid].DamageRate;
	if (skillid == Protocol::SKILL_BASE_DEFAULT_ATTACK)
	{
		float Rate = ((isCritical) ? GetCriticalRate() : 1);
		return (GetDamage() * DamageRate) * Rate;
	}
	
	return 0;
}

bool Player::IsCritical()
{
	return (rand() % 100) < (GetCriticalChane() * 100); // 50% Ȯ�� ����

}

void Player::UseItem(Protocol::C_USEINVENTORY& pkt)
{
    ServerItemData* item = ItemUtils::GetItem(pkt.code());
    if (item == nullptr) {
        LOG("Fail Used Item");
        return;
    }

    //�κ��丮 ���
    Inventory->UseInventoryItem(pkt.instance_id(), pkt.code());

    if (item->Type == ItemType::Equipment)
    {

        bool bEqu = Equipment->SetEquimentItem(item->slot, pkt.instance_id(), item->code);
        if (bEqu) {
            objectInfo->set_maxhp(GetPlayerMaxHp());
            int curHp = std::min<int>(objectInfo->hp(), GetPlayerMaxHp());
            objectInfo->set_hp(curHp);
            
            // TODO ���� �ưų�, �����ִ� ��� ���� ����ä�λ���鿡�� ����
            Protocol::S_LOADEQUIPMENT loadPkt;
            Protocol::ObjectInfo* object = loadPkt.mutable_objectinfo();
            object->CopyFrom(*objectInfo);
            
            //�������� ������ ����
            for (auto& item : Equipment->GetEquipments()) {
                Protocol::ItemData* addItem = loadPkt.add_items();
                addItem->CopyFrom(item);
            }

            RoomRef Room = room.lock();
            if (Room == nullptr)
                return;

            SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(loadPkt);
            Room->Broadcast(sendBuffer);
        }

    }
    else if (item->Type == ItemType::Consumable) {
        //�Ҹ�ǰ ���
        int32 hp = objectInfo->hp() + item->Heal;
        if (objectInfo->maxhp() <= hp) {
            objectInfo->set_hp(objectInfo->maxhp());
        }
        else{ objectInfo->set_hp(hp); }

        Protocol::S_USEINVENTORY usePkt;
        Protocol::ObjectInfo* object = usePkt.mutable_objectinfo();
        object->CopyFrom(*objectInfo);
        usePkt.set_code(pkt.code());
        RoomRef Room = room.lock();
        if (Room == nullptr)
            return;

        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(usePkt);
        Room->Broadcast(sendBuffer);
    }

}

FSkillData Player::GetSkilIData(Protocol::SkillID skillid)
{

	return SkillDatabase[skillid];
}

