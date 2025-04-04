#pragma once
#include "Protocol.pb.h"
#include <string>
#include "ItemUtils.h"
#include "PlayerInventory.h"
class CharacterDB
{
public:
	/**************************************
					캐릭터 생성
	**************************************/
	static void InsertCreateCharacter(int64 dbid, string name);


	/**************************************
				유저 케릭터 정보 
	**************************************/
	static Protocol::ObjectInfo SelectCharacterInfoToName(string name);
	static Protocol::ObjectInfo* SelectCharacterInfoToID(int64 DBID, int64 characterIndex);
	

	/**************************************
			유저 종료시 위치저장
	**************************************/
	static void UpdatePlayerPos(Protocol::ObjectInfo* objectInfo, uint64 characterDBID);


	/**************************************
			유저 골드 획득 및 소모
	**************************************/
	static void UpdatePlayerGold(uint64 characterDBID, int gainGold, int logType);


	/**************************************
			유저 경험치 및 레벨업
	**************************************/
	static void UpdatePlayerExp(uint64 characterDBID, int nowExp);
	static void UpdatePlayerLevelUp(uint64 characterDBID, int level, int nowExp);


    /**************************************
            유저 장비 및 인벤토리 로드
    **************************************/
    static void GetPlayerEquipment(GameSessionRef gamesession,  uint64 characterDBID);
    static void GetPlayerInventory(GameSessionRef gamesession, uint64 characterDBID);


    /**************************************
        유저 인벤토리 습득 및 업데이트
    **************************************/
    static void UpdatePlayerInventoryItem(uint64 characterDBID, Protocol::C_DB_UPDATEINVENTORY pkt);
    static void InsterPlayerInventoryItem(uint64 characterDBID, Protocol::C_DB_INSERTINVENTORY pkt);
    static void DeletePlayerInventoryItem(int64 instanceID);
    static void UpdatePlayerEquipment(Protocol::C_DB_EQUIPMENT& pkt);


    static std::unordered_map<int32, std::weak_ptr<GameSession>> g_requestMap;
};

