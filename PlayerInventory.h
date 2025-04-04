#pragma once
#include <unordered_map>
#include "ItemUtils.h"
#include "DBProtocol.pb.h"
struct PlayerItemInfo
{
    uint64 id = 0;
    uint64 code = 0;
    int32 quantity = 0;
    int32 enhancement_level = 0;
    int32 durability = 0;
    int32 slot_index = 0;
    bool is_equipped = 0;
    bool is_new = false;        // True일경우 DB서버에 Insert함
    bool is_change = false;        // True일경우 DB서버에 아이템을 업로드함.
};

class PlayerInventory
{
public:
    PlayerInventory();
    ~PlayerInventory();

public:
    void UpdateInventory(uint64 characterID);                   // DB서버에 업데이트 요청
    void DropItem(ServerItemData item);                         // 아이템 습득 처리
    bool GetLoadInventory(){return loadInventory;}              // 인벤토리가 로드가 된적이 있는지 확인
    void LoadInventory(Protocol::S_DB_LOADINVENTORY& pkt);      // DB서버에서 받아온 데이터 게임서버로 저장
    Protocol::S_LOADINVENTORY SendInventoryPkt();               // 맵 이동시 인벤토리 데이터 다시 전송할때 사용
    void DeleteItem(int foundIndex);                            // 소모품 다 사용했을때
    bool UseInventoryItem(int64 instanceID, int itemcode);      // 장착 및 소모품 사용
private:
    void ConvertItemDataToPlayerItemInfo(int32 slotIndex, PlayerItemInfo& playerInfo, ServerItemData item); // 변환작업
    void ConvertPlayerItemInfoToItemData(Protocol::ItemData* item, PlayerItemInfo playerInfo);              // 변환작업
    int FoundItemIndex(int64 instanceID);                       // 인벤토리에 아이템이있는지
private:
    int inventoryCnt = 35;                                      // 5(Col) * 7(Row)               
    unordered_map<int, PlayerItemInfo> inventory;               // 아이템값
    bool loadInventory = false;                                 // 한번 DB서버에서 값을 불러오면 게임서버 내부에서 처리
};

