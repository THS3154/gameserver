#include "pch.h"
#include "PlayerInventory.h"
#include "CharacterDB.h"
PlayerInventory::PlayerInventory()
{
    for (int i = 0; i < inventoryCnt; i++) {
        PlayerItemInfo item;
        inventory[i] = item;
    }
}

PlayerInventory::~PlayerInventory()
{
}

void PlayerInventory::UpdateInventory(uint64 characterID)
{
    // DB�� ������Ʈ ��û
    Protocol::C_DB_INSERTINVENTORY insertPkt;
    Protocol::C_DB_UPDATEINVENTORY updatePkt;

    for (const auto& [key, item] : inventory)
    {
        if (item.is_new) {
            // �ű� ������
            Protocol::ItemData* itemData = insertPkt.add_items();
            ConvertPlayerItemInfoToItemData(itemData, item);
        }
        else if (item.is_change) {
            // ������ �����ϴ� ��
            Protocol::ItemData* itemData = updatePkt.add_items();
            ConvertPlayerItemInfoToItemData(itemData, item);
        }
    }

    CharacterDB::InsterPlayerInventoryItem(characterID, insertPkt);
    CharacterDB::UpdatePlayerInventoryItem(characterID, updatePkt);
}

void PlayerInventory::DropItem(ServerItemData item)
{
    // ������ ����

    // TODO ���� �������� �Ҹ�ǰ����
    bool bConsumable = item.Type == ItemType::Consumable;
    
    // TODO �ش� �Ҹ�ǰ�� �κ��丮�� �����ϴ��� �׸��� ���� ����� �󽽷� ã��
    uint64 targetCode = item.code;
    bool found = false;
    bool spaceSlot = false;
    bool bOver = false;
    int index = -1;
    for (const auto& [key, item] : inventory)
    {
        if (bConsumable) {
            if (item.code == 0 && spaceSlot == false) {
                index = key;        // �ε�����
                spaceSlot = true;
            }

            if (item.code == targetCode)
            {
                index = key;        // �ε�����
                found = true;       // ã�Ҵ���?
                break;  // �Ҹ�ǰ�� ����ġ���ٰ� ���� ����Ǵѱ� ����.
            }
        }
        else {
            if (item.code == 0) {
                index = key;        // �ε�����
                break;
            }
        }
    }

    

    if (bConsumable) {
        // �Ҹ�ǰ
        if (found) {
            // ����
            inventory[index].quantity++;
            inventory[index].is_change = true;  // true�� �ؾ� ���߿� DB�� ������Ʈ ��
            return ;
        }
    }

    //���� �߰�
    if (index == -1) {
        // �κ� ����
        bOver = true;
        return;
    }
    ConvertItemDataToPlayerItemInfo(index, inventory[index], item);
}

void PlayerInventory::LoadInventory(Protocol::S_DB_LOADINVENTORY& pkt)
{
    for (auto& pktItem : pkt.items()) {
        PlayerItemInfo item;
        item.id                 = pktItem.id();
        item.code               = pktItem.code();
        item.quantity           = pktItem.quantity();
        item.enhancement_level  = pktItem.enhancement_level();
        item.durability         = pktItem.durability();
        item.slot_index         = pktItem.slot_index();
        item.is_equipped        = pktItem.is_equipped();
        inventory[pktItem.slot_index()] = item;
    }

    // Load�ߴٰ� �˸�
    loadInventory = true;
}

Protocol::S_LOADINVENTORY PlayerInventory::SendInventoryPkt()
{
    Protocol::S_LOADINVENTORY pkt;
    for (auto& item : inventory) {
        Protocol::ItemData* myItem = pkt.add_items();
        ConvertPlayerItemInfoToItemData(myItem, item.second);   // �κ��丮 �����͸� ����
    }
    return pkt;
}

void PlayerInventory::DeleteItem(int foundIndex)
{
    PlayerItemInfo Keep = inventory[foundIndex];
    PlayerItemInfo item;
    inventory[foundIndex] = item;

    // TODO DB�� Delete ��Ŷ����
    CharacterDB::DeletePlayerInventoryItem(Keep.id);
}


bool PlayerInventory::UseInventoryItem(int64 instanceID, int itemcode)
{
    int foundIndex = FoundItemIndex(instanceID);
    
    // �ش�������� ��ã��. �߸��� ��ûó��
    if (foundIndex == -1) {
        LOG("NotFoundItem");
        return false;
    }

    ServerItemData* item = ItemUtils::GetItem(itemcode);

    if(item->Type == ItemType::Consumable){
        // �Ҹ�ǰ ���
        inventory[foundIndex].quantity--;
        inventory[foundIndex].is_change = true;
        if (inventory[foundIndex].quantity <= 0) {
            // �Ҹ�ǰ �ٻ�� �κ��丮 ����
            DeleteItem(foundIndex);
        }
    }
    else if (item->Type == ItemType::Equipment)
    {
        // �������
        inventory[foundIndex].is_equipped = !inventory[foundIndex].is_equipped;
        inventory[foundIndex].is_change = true;
    }
    LOG("UsedItem InstanceID : " << inventory[foundIndex].id << " ItemName : " << item->Name << " Item IsEquipment : " << inventory[foundIndex].is_equipped);
    return true;
}

void PlayerInventory::ConvertItemDataToPlayerItemInfo(int32 slotIndex, PlayerItemInfo& playerInfo, ServerItemData item)
{
    playerInfo.code = item.code;
    playerInfo.quantity = 1;
    playerInfo.slot_index = slotIndex;
    playerInfo.is_new = true;

}

void PlayerInventory::ConvertPlayerItemInfoToItemData(Protocol::ItemData* item, PlayerItemInfo playerInfo)
{
    item->set_id(playerInfo.id);
    item->set_code(playerInfo.code);
    item->set_quantity(playerInfo.quantity);
    item->set_enhancement_level(playerInfo.enhancement_level);
    item->set_slot_index(playerInfo.slot_index);
    item->set_is_equipped(playerInfo.is_equipped);
}

int PlayerInventory::FoundItemIndex(int64 instanceID)
{
    int foundIndex = -1;

    for (const auto& [key, item] : inventory)
    {
        if (instanceID == item.id) {
            foundIndex = key;
            break;
        }
    }

    return foundIndex;
}
