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
    // DB에 업데이트 요청
    Protocol::C_DB_INSERTINVENTORY insertPkt;
    Protocol::C_DB_UPDATEINVENTORY updatePkt;

    for (const auto& [key, item] : inventory)
    {
        if (item.is_new) {
            // 신규 아이템
            Protocol::ItemData* itemData = insertPkt.add_items();
            ConvertPlayerItemInfoToItemData(itemData, item);
        }
        else if (item.is_change) {
            // 기존에 존재하는 템
            Protocol::ItemData* itemData = updatePkt.add_items();
            ConvertPlayerItemInfoToItemData(itemData, item);
        }
    }

    CharacterDB::InsterPlayerInventoryItem(characterID, insertPkt);
    CharacterDB::UpdatePlayerInventoryItem(characterID, updatePkt);
}

void PlayerInventory::DropItem(ServerItemData item)
{
    // 아이템 얻음

    // TODO 얻은 아이템이 소모품인지
    bool bConsumable = item.Type == ItemType::Consumable;
    
    // TODO 해당 소모품이 인벤토리에 존재하는지 그리고 제일 가까운 빈슬롯 찾기
    uint64 targetCode = item.code;
    bool found = false;
    bool spaceSlot = false;
    bool bOver = false;
    int index = -1;
    for (const auto& [key, item] : inventory)
    {
        if (bConsumable) {
            if (item.code == 0 && spaceSlot == false) {
                index = key;        // 인덱스값
                spaceSlot = true;
            }

            if (item.code == targetCode)
            {
                index = key;        // 인덱스값
                found = true;       // 찾았는지?
                break;  // 소모품은 그위치에다가 값을 쓰면되닌깐 나감.
            }
        }
        else {
            if (item.code == 0) {
                index = key;        // 인덱스값
                break;
            }
        }
    }

    

    if (bConsumable) {
        // 소모품
        if (found) {
            // 더함
            inventory[index].quantity++;
            inventory[index].is_change = true;  // true로 해야 나중에 DB에 업데이트 됨
            return ;
        }
    }

    //새로 추가
    if (index == -1) {
        // 인벤 꽉참
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

    // Load했다고 알림
    loadInventory = true;
}

Protocol::S_LOADINVENTORY PlayerInventory::SendInventoryPkt()
{
    Protocol::S_LOADINVENTORY pkt;
    for (auto& item : inventory) {
        Protocol::ItemData* myItem = pkt.add_items();
        ConvertPlayerItemInfoToItemData(myItem, item.second);   // 인벤토리 데이터를 넣음
    }
    return pkt;
}

void PlayerInventory::DeleteItem(int foundIndex)
{
    PlayerItemInfo Keep = inventory[foundIndex];
    PlayerItemInfo item;
    inventory[foundIndex] = item;

    // TODO DB에 Delete 패킷전송
    CharacterDB::DeletePlayerInventoryItem(Keep.id);
}


bool PlayerInventory::UseInventoryItem(int64 instanceID, int itemcode)
{
    int foundIndex = FoundItemIndex(instanceID);
    
    // 해당아이템을 못찾음. 잘못된 요청처리
    if (foundIndex == -1) {
        LOG("NotFoundItem");
        return false;
    }

    ServerItemData* item = ItemUtils::GetItem(itemcode);

    if(item->Type == ItemType::Consumable){
        // 소모품 사용
        inventory[foundIndex].quantity--;
        inventory[foundIndex].is_change = true;
        if (inventory[foundIndex].quantity <= 0) {
            // 소모품 다사용 인벤토리 제거
            DeleteItem(foundIndex);
        }
    }
    else if (item->Type == ItemType::Equipment)
    {
        // 장착장비
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
