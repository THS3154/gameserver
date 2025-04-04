#include "pch.h"
#include "CharacterDB.h"
#include "DBConnection.h"
#include "DBBind.h"
#include "DBConnectionPool.h"
#include "DBManager.h"
#include "DBServerPacketHandler.h"
#include "ObjectUtils.h"
#include <unordered_map>

std::unordered_map<int32, std::weak_ptr<GameSession>> CharacterDB::g_requestMap;

void CharacterDB::InsertCreateCharacter(int64 dbid, string name)
{
	DBConnection* dbConn = GDBConnectionPool->Pop();
	// TODO ���߿� �����߰� character_type
	DBBind<2, 0> dbBind(*dbConn, "INSERT INTO characterlist(user_id, name) VALUES(?, ?)");

	// ������ ���ε� 
	std::vector<char> bufferName(name.begin(), name.end());
	bufferName.push_back('\0'); // NULL ���� �߰�

	dbBind.BindParam(0, dbid);
	dbBind.BindParam(1, bufferName.data(), bufferName.size());

	ASSERT_CRASH(dbBind.Execute());
	dbConn->Unbind();
	GDBConnectionPool->Push(dbConn);

}

Protocol::ObjectInfo CharacterDB::SelectCharacterInfoToName(string name)
{
	Protocol::ObjectInfo pkt;

	// DB�� ����� ĳ���� ����� ������
	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBBind<1, 8> dbBind(*dbConn, "SELECT name, character_type, level, exp, pos_map, pos_x, pos_y, pos_z FROM characterlist WHERE name = ?");
	std::vector<char> bufferName(name.begin(), name.end());
	bufferName.push_back('\0'); // NULL ���� �߰�
	
	dbBind.BindParam(0, bufferName.data(), name.size());

	char outname[256] = { 0 };
	int32 out_character_type;
	int32 out_level;
	int32 out_map;
	int32 out_exp;
	float out_x;
	float out_y;
	float out_z;

	dbBind.BindCol(0, OUT outname,255);
	dbBind.BindCol(1, OUT out_character_type);
	dbBind.BindCol(2, OUT out_level);
	dbBind.BindCol(3, OUT out_exp);
	dbBind.BindCol(4, OUT out_map);
	dbBind.BindCol(5, OUT out_x);
	dbBind.BindCol(6, OUT out_y);
	dbBind.BindCol(7, OUT out_z);

	ASSERT_CRASH(dbBind.Execute());
	while (dbConn->Fetch())
	{
		string out_name(outname);
		Protocol::PosInfo* posInfo = pkt.mutable_posinfo();
		posInfo->set_x(out_x);
		posInfo->set_y(out_y);
		posInfo->set_z(out_z);
		pkt.set_name(out_name);
		pkt.set_playerlevel(out_level);
		pkt.set_curexp(out_exp);
		pkt.set_leveltype(static_cast<Protocol::LevelType>(out_map));
		//player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);
	}

	GDBConnectionPool->Push(dbConn);

    return pkt;
}

Protocol::ObjectInfo* CharacterDB::SelectCharacterInfoToID(int64 DBID, int64 characterIndex)
{
	Protocol::ObjectInfo* pkt = new Protocol::ObjectInfo();

	// DB�� ����� ĳ���� ����� ������
	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBBind<2, 9> dbBind(*dbConn, "SELECT name, character_type, level, gold, exp, pos_map, pos_x, pos_y, pos_z FROM characterlist WHERE user_id = ? AND character_id = ?");

	dbBind.BindParam(0, DBID);
	dbBind.BindParam(1, characterIndex);
	char outname[256] = { 0 };
	
	int32 out_character_type;
	int32 out_level;
	int32 out_gold;
	int32 out_exp;
	int32 out_map;
	float out_x;
	float out_y;
	float out_z;

	dbBind.BindCol(0, OUT outname, 255);
	dbBind.BindCol(1, OUT out_character_type);
	dbBind.BindCol(2, OUT out_level);
	dbBind.BindCol(3, OUT out_gold);
	dbBind.BindCol(4, OUT out_exp);
	dbBind.BindCol(5, OUT out_map);
	dbBind.BindCol(6, OUT out_x);
	dbBind.BindCol(7, OUT out_y);
	dbBind.BindCol(8, OUT out_z);

	ASSERT_CRASH(dbBind.Execute());

	


	while (dbConn->Fetch())
	{
		string out_name(outname);
		Protocol::PosInfo* posInfo = pkt->mutable_posinfo();
		posInfo->set_x(out_x);
		posInfo->set_y(out_y);
		posInfo->set_z(out_z);
		pkt->set_name(out_name);
		pkt->set_playerlevel(out_level);
		pkt->set_curexp(out_exp);
		pkt->set_gold(out_gold);
		pkt->set_leveltype(static_cast<Protocol::LevelType>(out_map));
		//LOG("Enter UserName : " << out_name);
		//player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);
	}

	

	GDBConnectionPool->Push(dbConn);

	if (dbConn->GetRowCount() == 0) {
		return nullptr;
	}

	return pkt;
}

void CharacterDB::UpdatePlayerPos(Protocol::ObjectInfo* objectInfo, uint64 characterDBID)
{
	if(objectInfo == nullptr) return;
	Protocol::C_DB_UPDATEPOS pkt;
	Protocol::ObjectInfo* objectinfo = pkt.mutable_objectinfo();
	pkt.set_characterid(characterDBID);
	objectinfo->CopyFrom(*objectInfo);
    
    
	SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
	GDBManager.Broadcast(sendBuffer);
}

void CharacterDB::UpdatePlayerExp( uint64 characterDBID, int nowExp)
{

	Protocol::C_DB_GAINEXP pkt;
	pkt.set_characterid(characterDBID);
	pkt.set_level(0);
	pkt.set_nowexp(nowExp);
    
    

	SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
	GDBManager.Broadcast(sendBuffer);

	
}

void CharacterDB::UpdatePlayerLevelUp( uint64 characterDBID, int level, int nowExp)
{
	Protocol::C_DB_GAINEXP pkt;
	pkt.set_characterid(characterDBID);
	pkt.set_level(level);
	pkt.set_nowexp(nowExp);


	SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
	GDBManager.Broadcast(sendBuffer);
}

void CharacterDB::GetPlayerEquipment(GameSessionRef gamesession, uint64 characterDBID)
{

    int64 DBGenerator = ObjectUtils::s_DBGenerator.fetch_add(1);
    g_requestMap[DBGenerator] = gamesession;
    Protocol::C_DB_LOADEQUIPMENT pkt;
    pkt.set_characterid(characterDBID);
    pkt.set_requestid(DBGenerator);

    SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
    GDBManager.Broadcast(sendBuffer);
    
    // To -> S_LOADEQUIPMENT ������
}

void CharacterDB::GetPlayerInventory(GameSessionRef gamesession, uint64 characterDBID)
{
    // DB�������� �κ��丮 ������ ��û
    int64 DBGenerator = ObjectUtils::s_DBGenerator.fetch_add(1);
    g_requestMap[DBGenerator] = gamesession;
    Protocol::C_DB_LOADINVENTORY pkt;
    pkt.set_characterid(characterDBID);
    pkt.set_requestid(DBGenerator);
    SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
    GDBManager.Broadcast(sendBuffer);

    // To -> S_LOADINVENTORY ������
}

void CharacterDB::UpdatePlayerInventoryItem(uint64 characterDBID, Protocol::C_DB_UPDATEINVENTORY pkt)
{
    // TODO �Ҹ�ǰ ����� ���, �Ҹ�ǰ ���� ���
    pkt.set_characterid(characterDBID);

    SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
    GDBManager.Broadcast(sendBuffer);
}

void CharacterDB::InsterPlayerInventoryItem( uint64 characterDBID, Protocol::C_DB_INSERTINVENTORY pkt)
{
    // TODO �������� ������� ���
    pkt.set_characterid(characterDBID);

    SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
    GDBManager.Broadcast(sendBuffer);

}

void CharacterDB::DeletePlayerInventoryItem(int64 instanceID)
{
    Protocol::C_DB_DELETEINVENTORY pkt;
    pkt.set_instanceid(instanceID);
    SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
    GDBManager.Broadcast(sendBuffer);
}

void CharacterDB::UpdatePlayerEquipment(Protocol::C_DB_EQUIPMENT& pkt)
{
    SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
    GDBManager.Broadcast(sendBuffer);
}




void CharacterDB::UpdatePlayerGold( uint64 characterDBID, int gainGold, int logType)
{
	//��� �������
	Protocol::C_DB_GAINGOLD pkt;
	pkt.set_characterid(characterDBID);
	pkt.set_gold(gainGold);
	pkt.set_logtype(logType);


	SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt);
	GDBManager.Broadcast(sendBuffer);

	
}
