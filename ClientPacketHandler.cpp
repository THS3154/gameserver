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



// 직접 컨텐츠 작업자
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}


bool Handle_C_CREATECHARACTER(PacketSessionRef& session, Protocol::C_CREATECHARACTER& pkt) {
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	// TODO 유저가 캐릭터 생성 패킷을 보냄
	// 이름 중복체크를 해서 넘겨줘야함

	int row = -1;
	uint64 db_userid = gameSession->GetDBID();

	Protocol::S_CREATECHARACTER createPkt;
	createPkt.set_success(false);

	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBBind<1, 1> dbBind(*dbConn, "SELECT user_id FROM characterlist WHERE name = ?");
		
		const string name = pkt.name();
		std::vector<char> bufferName(name.begin(), name.end());
		bufferName.push_back('\0'); // NULL 종료 추가

		int out_userid;
		dbBind.BindCol(0, OUT out_userid);


		dbBind.BindParam(0, bufferName.data(), bufferName.size());
		ASSERT_CRASH(dbBind.Execute());
		
		// 닉네임 중복아님 캐릭생성가능
		row = dbConn->GetRowCount();
		if (row == 0)
		{
			// 캐릭생성가능
			createPkt.set_success(true);
			createPkt.set_message(Protocol::CHARACTER_SUCCESS);

			//데이터를 넣음
			CharacterDB::InsertCreateCharacter(db_userid, name);

			//데이터를 불러옴
			Protocol::ObjectInfo createOjbect = CharacterDB::SelectCharacterInfoToName(name);
			Protocol::ObjectInfo* objectinfo = createPkt.mutable_objectinfo();
			objectinfo->CopyFrom(createOjbect);
		}
		else {
			//닉네임 중복오류
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
	// TODO : Validation 체크


    GLoginManager->HandleLogin(gameSession, pkt);

	

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = ObjectUtils::CreatePlayer(gameSession, pkt.playerindex());

	if (player == nullptr) {
		// 잘못된 접근임
		LOG("잘못된 접근");
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
    // TODO 인벤토리 정보 로드

    return false;
}

bool Handle_C_USEINVENTORY(PacketSessionRef& session, Protocol::C_USEINVENTORY& pkt)
{
    // TODO 장비창 사용
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
    // TODO 장착중인 장비로드

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
	// TODO 마지막 플레이어 위치 DB 저장
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

	// 입장 레벨 값
	Protocol::LevelType levelType = pkt.leveltype();

	ObjectRef object = std::static_pointer_cast<Object>(player);

	Protocol::S_LEVEL_MOVE levelMove;
	
	if (auto level = GMapManager->GetRoomManager(levelType)) {
		levelMove.set_success(true);
		levelMove.set_leveltype(pkt.leveltype());
		levelMove.set_levelname(level->GetLevelName());
		// 클라에서 받은 위치값
		
		object->movePosInfo->set_x(pkt.dest_x());
		object->movePosInfo->set_y(pkt.dest_y());
		object->movePosInfo->set_z(pkt.dest_z());
		

	}
	else {
		// 잘못된 맵 정보
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

	// 입장 레벨 값
	Protocol::LevelType levelType = pkt.leveltype();

	ObjectRef object = std::static_pointer_cast<Object>(player);
	
	// 기존 채널에 있는 사람에게 떠난다고 알림
	GMapManager->DoAsync(&MapManager::LeaveMap, object);
	object->posInfo->set_x(object->movePosInfo->x());
	object->posInfo->set_y(object->movePosInfo->y());
	object->posInfo->set_z(object->movePosInfo->z());
	// 해당 레벨로 입장.
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

	// 보낸애 빼고 다 메세지 보냄
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

	//공격 전달
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
