#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib,"ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib,"Protobuf\\Debug\\libprotobufd.lib")
#pragma comment(lib,"ssl\\Debug\\libcrypto.lib")
#else
#pragma comment(lib,"ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib,"Protobuf\\Release\\libprotobuf.lib")
#pragma comment(lib,"ssl\\Release\\libcrypto.lib")
#endif

#include "CorePch.h"
#include "Enum.pb.h"
#include "Struct.pb.h"
#include "DBProtocol.pb.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"


using ClientSessionRef = std::shared_ptr<class GameSession>;
using GameSessionRef = std::shared_ptr<class GameSession>;
using PlayerRef = std::shared_ptr<class Player>;
using ObjectRef = std::shared_ptr<class Object>;
using CreatureRef = std::shared_ptr<class Creature>;
using MonsterRef = std::shared_ptr<class Monster>;
using RoomRef = std::shared_ptr<class Room>;
using RoomManagerRef = std::shared_ptr<class RoomManager>;
using MapManagerRef = std::shared_ptr<class MapManager>;
using LoginManagerRef = std::shared_ptr<class LoginManager>;


#define SEND_PACKET(pkt)													\
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);	\
	session->Send(sendBuffer);