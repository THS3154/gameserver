#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include <string>
#include "RoomManager.h"
#include "MapManager.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "GameSession.h"
#include "ObjectUtils.h"
#include "CharacterDB.h"
#include "Protocol.pb.h"
#include "LoginManager.h"
#include "Utils.h"
static Atomic<uint64> idGenerator = 1;
PacketHandlerFunc GPacketHandler[UINT16_MAX];
void ConvertStringToWCHAR(const std::string& str, WCHAR* dest, size_t destSize)
{
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, dest, destSize);
}

string ConvertWCHARToString(const WCHAR* wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], size_needed, NULL, NULL);
	return str;
}



// ���� ������ �۾���
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}


bool Handle_C_CREATECHARACTER(PacketSessionRef& session, Protocol::C_CREATECHARACTER& pkt) {
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	// TODO ������ ĳ���� ���� ��Ŷ�� ����
	// �̸� �ߺ�üũ�� �ؼ� �Ѱ������

	int row = -1;
	uint64 db_userid = gameSession->GetDBID();

	Protocol::S_CREATECHARACTER createPkt;
	createPkt.set_success(false);

	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBBind<1, 1> dbBind(*dbConn, "SELECT user_id FROM characterlist WHERE name = ?");
		
		const string name = pkt.name();
		std::vector<char> bufferName(name.begin(), name.end());
		bufferName.push_back('\0'); // NULL ���� �߰�

		int out_userid;
		dbBind.BindCol(0, OUT out_userid);


		dbBind.BindParam(0, bufferName.data(), bufferName.size());
		ASSERT_CRASH(dbBind.Execute());
		
		// �г��� �ߺ��ƴ� ĳ����������
		row = dbConn->GetRowCount();
		if (row == 0)
		{
			// ĳ����������
			createPkt.set_success(true);
			createPkt.set_message(Protocol::CHARACTER_SUCCESS);

			//�����͸� ����
			CharacterDB::InsertCreateCharacter(db_userid, name);

			//�����͸� �ҷ���
			Protocol::ObjectInfo createOjbect = CharacterDB::SelectCharacterInfoToName(name);
			Protocol::ObjectInfo* objectinfo = createPkt.mutable_objectinfo();
			objectinfo->CopyFrom(createOjbect);
		}
		else {
			//�г��� �ߺ�����
			createPkt.set_message(Protocol::CHARACTER_ERROR_ID_DUPLE);
		}

		dbConn->Unbind();
		GDBConnectionPool->Push(dbConn);
	}
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(createPkt);
	gameSession->Send(sendBuffer);
	return true;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	// TODO : Validation üũ


    GLoginManager->HandleLogin(gameSession, pkt);

	

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = ObjectUtils::CreatePlayer(gameSession, pkt.playerindex());

	if (player == nullptr) {
		// �߸��� ������
		LOG("�߸��� ����");
		return false;
	}
	Protocol::ObjectInfo* objectinfo = player->objectInfo;
	
	objectinfo->set_leveltype(pkt.leveltype());
	//GRoom->DoAsync(&Room::HandleEnterPlayer, player);
	GMapManager->DoAsync(&MapManager::JoinMap, pkt.leveltype(), std::static_pointer_cast<Object>(player), true);
	
	return true;
}

bool Handle_C_LOADINVENTORY(PacketSessionRef& session, Protocol::C_LOADINVENTORY& pkt)
{
    // TODO �κ��丮 ���� �ε�

    return false;
}

bool Handle_C_USEINVENTORY(PacketSessionRef& session, Protocol::C_USEINVENTORY& pkt)
{
    // TODO ���â ���
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    PlayerRef player = gameSession->player;
    if (player == nullptr)
        return false;

    RoomRef room = player->room.lock();
    if (room == nullptr)
        return false;


    player->UseItem(pkt);
    return false;
}




bool Handle_C_LOADEQUIPMENT(PacketSessionRef& session, Protocol::C_LOADEQUIPMENT& pkt)
{
    // TODO �������� ���ε�

    return false;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->player;
	if (player == nullptr)
		return false;
	
	Protocol::ObjectInfo* objectinfo = player->objectInfo;
	uint64 characterDBID = gameSession->myCharacterDBID[objectinfo->object_id()];


	RoomRef room = player->room.lock();
	if(room == nullptr)
		return false;

	//GRoom->DoAsync(&Room::HandleLeavePlayer, player);
	GMapManager->GetRoomManager(player)->DoAsync(&RoomManager::LeaveChanel, player);
	CharacterDB::UpdatePlayerPos(objectinfo, characterDBID);
	// TODO ������ �÷��̾� ��ġ DB ����
	return true;
}

bool Handle_C_SPAWN_RESERVE(PacketSessionRef& session, Protocol::C_SPAWN_RESERVE& pkt)
{
    return false;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->player;
	if (player == nullptr)
		return false;
	
	RoomRef room = player->room.lock();
	if (room == nullptr)
		return false;

	GMapManager->GetRoom(player)->DoAsync(&Room::HandleMove, pkt);
	//GRoom->DoAsync(&Room::HandleMove, pkt);
	return true;
}

bool Handle_C_LEVEL_MOVE(PacketSessionRef& session, Protocol::C_LEVEL_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->player;
	if (player == nullptr)
		return false;

	// ���� ���� ��
	Protocol::LevelType levelType = pkt.leveltype();

	ObjectRef object = std::static_pointer_cast<Object>(player);

	Protocol::S_LEVEL_MOVE levelMove;
	
	if (auto level = GMapManager->GetRoomManager(levelType)) {
		levelMove.set_success(true);
		levelMove.set_leveltype(pkt.leveltype());
		levelMove.set_levelname(level->GetLevelName());
		// Ŭ�󿡼� ���� ��ġ��
		
		object->movePosInfo->set_x(pkt.dest_x());
		object->movePosInfo->set_y(pkt.dest_y());
		object->movePosInfo->set_z(pkt.dest_z());
		

	}
	else {
		// �߸��� �� ����
		levelMove.set_success(false);
		levelMove.set_leveltype(Protocol::LEVEL_TYPE_NONE);
		levelMove.set_levelname("none");
	}

	auto  sendBuffer = ClientPacketHandler::MakeSendBuffer(levelMove);
	gameSession->Send(sendBuffer);
	return true;
}

bool Handle_C_LEVEL_MOVE_COMPLETE(PacketSessionRef& session, Protocol::C_LEVEL_MOVE_COMPLETE& pkt)
{

	auto gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->player;
	if (player == nullptr)
		return false;

	RoomRef room = player->room.lock();
	if (room == nullptr)
		return false;

	// ���� ���� ��
	Protocol::LevelType levelType = pkt.leveltype();

	ObjectRef object = std::static_pointer_cast<Object>(player);
	
	// ���� ä�ο� �ִ� ������� �����ٰ� �˸�
	GMapManager->DoAsync(&MapManager::LeaveMap, object);
	object->posInfo->set_x(object->movePosInfo->x());
	object->posInfo->set_y(object->movePosInfo->y());
	object->posInfo->set_z(object->movePosInfo->z());
	// �ش� ������ ����.
	GMapManager->DoAsync(&MapManager::JoinMap, levelType, object, false);

	Protocol::S_LEVEL_MOVE_COMPLETE completePkt;
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(completePkt);
	session->Send(sendBuffer);
	return false;
}


bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	

	PlayerRef player = gameSession->player;

	//LOG("ObjectID : " << gameSession->player->objectInfo->object_id() << " Level : " << player->objectInfo->leveltype() << " Chanel : " << player->objectInfo->chanel() << " Messge : " << pkt.msg());

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	chatPkt.set_playerid(player->objectInfo->object_id());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	// ������ ���� �� �޼��� ����
	GMapManager->GetRoom(player)->DoAsync(&Room::Broadcast, sendBuffer, player->objectInfo->object_id());
	//GRoom->Broadcast(sendBuffer, player->objectInfo->object_id());

	return true;
}


bool Handle_C_SKILL(PacketSessionRef& session, Protocol::C_SKILL& pkt) {

	auto gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->player;
	if (player == nullptr)
		return false;

	RoomRef room = player->room.lock();
	if (room == nullptr)
		return false;

	//���� ����
	GMapManager->GetRoom(player)->DoAsync(&Room::HandleSkill, pkt);

	return true;
}

bool Handle_C_GAINEXP(PacketSessionRef& session, Protocol::C_GAINEXP& pkt)
{
	return false;
}

bool Handle_C_PING(PacketSessionRef& session, Protocol::C_PING& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    uint64 time = Utils::Millis();
    gameSession->SetSendTime(time);
    if (gameSession->player) {
        gameSession->player->pingTime = time;
    }
    return false;
}


bool Handle_C_DUNJEON(PacketSessionRef& session, Protocol::C_DUNJEON& pkt)
{
    //GameSessionRef gameSession = static_pointer_cast<GameSession>(session);


    return false;
}
