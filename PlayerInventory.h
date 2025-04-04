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
    bool is_new = false;        // True�ϰ�� DB������ Insert��
    bool is_change = false;        // True�ϰ�� DB������ �������� ���ε���.
};

class PlayerInventory
{
public:
    PlayerInventory();
    ~PlayerInventory();

public:
    void UpdateInventory(uint64 characterID);                   // DB������ ������Ʈ ��û
    void DropItem(ServerItemData item);                         // ������ ���� ó��
    bool GetLoadInventory(){return loadInventory;}              // �κ��丮�� �ε尡 ������ �ִ��� Ȯ��
    void LoadInventory(Protocol::S_DB_LOADINVENTORY& pkt);      // DB�������� �޾ƿ� ������ ���Ӽ����� ����
    Protocol::S_LOADINVENTORY SendInventoryPkt();               // �� �̵��� �κ��丮 ������ �ٽ� �����Ҷ� ���
    void DeleteItem(int foundIndex);                            // �Ҹ�ǰ �� ���������
    bool UseInventoryItem(int64 instanceID, int itemcode);      // ���� �� �Ҹ�ǰ ���
private:
    void ConvertItemDataToPlayerItemInfo(int32 slotIndex, PlayerItemInfo& playerInfo, ServerItemData item); // ��ȯ�۾�
    void ConvertPlayerItemInfoToItemData(Protocol::ItemData* item, PlayerItemInfo playerInfo);              // ��ȯ�۾�
    int FoundItemIndex(int64 instanceID);                       // �κ��丮�� ���������ִ���
private:
    int inventoryCnt = 35;                                      // 5(Col) * 7(Row)               
    unordered_map<int, PlayerItemInfo> inventory;               // �����۰�
    bool loadInventory = false;                                 // �ѹ� DB�������� ���� �ҷ����� ���Ӽ��� ���ο��� ó��
};

