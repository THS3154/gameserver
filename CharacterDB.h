#pragma once
#include "Protocol.pb.h"
#include <string>
#include "ItemUtils.h"
#include "PlayerInventory.h"
class CharacterDB
{
public:
	/**************************************
					ĳ���� ����
	**************************************/
	static void InsertCreateCharacter(int64 dbid, string name);


	/**************************************
				���� �ɸ��� ���� 
	**************************************/
	static Protocol::ObjectInfo SelectCharacterInfoToName(string name);
	static Protocol::ObjectInfo* SelectCharacterInfoToID(int64 DBID, int64 characterIndex);
	

	/**************************************
			���� ����� ��ġ����
	**************************************/
	static void UpdatePlayerPos(Protocol::ObjectInfo* objectInfo, uint64 characterDBID);


	/**************************************
			���� ��� ȹ�� �� �Ҹ�
	**************************************/
	static void UpdatePlayerGold(uint64 characterDBID, int gainGold, int logType);


	/**************************************
			���� ����ġ �� ������
	**************************************/
	static void UpdatePlayerExp(uint64 characterDBID, int nowExp);
	static void UpdatePlayerLevelUp(uint64 characterDBID, int level, int nowExp);


    /**************************************
            ���� ��� �� �κ��丮 �ε�
    **************************************/
    static void GetPlayerEquipment(GameSessionRef gamesession,  uint64 characterDBID);
    static void GetPlayerInventory(GameSessionRef gamesession, uint64 characterDBID);


    /**************************************
        ���� �κ��丮 ���� �� ������Ʈ
    **************************************/
    static void UpdatePlayerInventoryItem(uint64 characterDBID, Protocol::C_DB_UPDATEINVENTORY pkt);
    static void InsterPlayerInventoryItem(uint64 characterDBID, Protocol::C_DB_INSERTINVENTORY pkt);
    static void DeletePlayerInventoryItem(int64 instanceID);
    static void UpdatePlayerEquipment(Protocol::C_DB_EQUIPMENT& pkt);


    static std::unordered_map<int32, std::weak_ptr<GameSession>> g_requestMap;
};

