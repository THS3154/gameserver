#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "lllllllllqqq.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_CREATECHARACTER = 1002,
	PKT_S_CREATECHARACTER = 1003,
	PKT_C_ENTER_GAME = 1004,
	PKT_S_ENTER_GAME = 1005,
	PKT_C_LOADINVENTORY = 1006,
	PKT_S_LOADINVENTORY = 1007,
	PKT_C_LOADEQUIPMENT = 1008,
	PKT_S_LOADEQUIPMENT = 1009,
	PKT_C_USEINVENTORY = 1010,
	PKT_S_USEINVENTORY = 1011,
	PKT_C_LEAVE_GAME = 1012,
	PKT_S_LEAVE_GAME = 1013,
	PKT_C_SPAWN_RESERVE = 1014,
	PKT_S_SPAWN = 1015,
	PKT_S_DESPAWN = 1016,
	PKT_C_MOVE = 1017,
	PKT_S_MOVE = 1018,
	PKT_S_MOVES = 1019,
	PKT_C_LEVEL_MOVE = 1020,
	PKT_S_LEVEL_MOVE = 1021,
	PKT_C_LEVEL_MOVE_COMPLETE = 1022,
	PKT_S_LEVEL_MOVE_COMPLETE = 1023,
	PKT_C_CHAT = 1024,
	PKT_S_CHAT = 1025,
	PKT_C_SKILL = 1026,
	PKT_S_SKILL = 1027,
	PKT_C_GAINEXP = 1028,
	PKT_S_GAINEXP = 1029,
	PKT_S_GAINGOLD = 1030,
	PKT_C_PING = 1031,
	PKT_S_PING = 1032,
	PKT_C_DUNJEON = 1033,
	PKT_S_DUNJEON = 1034,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt);
bool Handle_C_CREATECHARACTER(PacketSessionRef& session, Protocol::C_CREATECHARACTER& pkt);
bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt);
bool Handle_C_LOADINVENTORY(PacketSessionRef& session, Protocol::C_LOADINVENTORY& pkt);
bool Handle_C_LOADEQUIPMENT(PacketSessionRef& session, Protocol::C_LOADEQUIPMENT& pkt);
bool Handle_C_USEINVENTORY(PacketSessionRef& session, Protocol::C_USEINVENTORY& pkt);
bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt);
bool Handle_C_SPAWN_RESERVE(PacketSessionRef& session, Protocol::C_SPAWN_RESERVE& pkt);
bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt);
bool Handle_C_LEVEL_MOVE(PacketSessionRef& session, Protocol::C_LEVEL_MOVE& pkt);
bool Handle_C_LEVEL_MOVE_COMPLETE(PacketSessionRef& session, Protocol::C_LEVEL_MOVE_COMPLETE& pkt);
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt);
bool Handle_C_SKILL(PacketSessionRef& session, Protocol::C_SKILL& pkt);
bool Handle_C_GAINEXP(PacketSessionRef& session, Protocol::C_GAINEXP& pkt);
bool Handle_C_PING(PacketSessionRef& session, Protocol::C_PING& pkt);
bool Handle_C_DUNJEON(PacketSessionRef& session, Protocol::C_DUNJEON& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_CREATECHARACTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_CREATECHARACTER>(Handle_C_CREATECHARACTER, session, buffer, len); };
		GPacketHandler[PKT_C_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ENTER_GAME>(Handle_C_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_C_LOADINVENTORY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOADINVENTORY>(Handle_C_LOADINVENTORY, session, buffer, len); };
		GPacketHandler[PKT_C_LOADEQUIPMENT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOADEQUIPMENT>(Handle_C_LOADEQUIPMENT, session, buffer, len); };
		GPacketHandler[PKT_C_USEINVENTORY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_USEINVENTORY>(Handle_C_USEINVENTORY, session, buffer, len); };
		GPacketHandler[PKT_C_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LEAVE_GAME>(Handle_C_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_C_SPAWN_RESERVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SPAWN_RESERVE>(Handle_C_SPAWN_RESERVE, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_LEVEL_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LEVEL_MOVE>(Handle_C_LEVEL_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_LEVEL_MOVE_COMPLETE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LEVEL_MOVE_COMPLETE>(Handle_C_LEVEL_MOVE_COMPLETE, session, buffer, len); };
		GPacketHandler[PKT_C_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_CHAT>(Handle_C_CHAT, session, buffer, len); };
		GPacketHandler[PKT_C_SKILL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SKILL>(Handle_C_SKILL, session, buffer, len); };
		GPacketHandler[PKT_C_GAINEXP] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_GAINEXP>(Handle_C_GAINEXP, session, buffer, len); };
		GPacketHandler[PKT_C_PING] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PING>(Handle_C_PING, session, buffer, len); };
		GPacketHandler[PKT_C_DUNJEON] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_DUNJEON>(Handle_C_DUNJEON, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CREATECHARACTER& pkt) { return MakeSendBuffer(pkt, PKT_S_CREATECHARACTER); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LOADINVENTORY& pkt) { return MakeSendBuffer(pkt, PKT_S_LOADINVENTORY); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LOADEQUIPMENT& pkt) { return MakeSendBuffer(pkt, PKT_S_LOADEQUIPMENT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_USEINVENTORY& pkt) { return MakeSendBuffer(pkt, PKT_S_USEINVENTORY); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_S_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_SPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_DESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_DESPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_S_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MOVES& pkt) { return MakeSendBuffer(pkt, PKT_S_MOVES); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LEVEL_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_S_LEVEL_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LEVEL_MOVE_COMPLETE& pkt) { return MakeSendBuffer(pkt, PKT_S_LEVEL_MOVE_COMPLETE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_S_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SKILL& pkt) { return MakeSendBuffer(pkt, PKT_S_SKILL); }
	static SendBufferRef MakeSendBuffer(Protocol::S_GAINEXP& pkt) { return MakeSendBuffer(pkt, PKT_S_GAINEXP); }
	static SendBufferRef MakeSendBuffer(Protocol::S_GAINGOLD& pkt) { return MakeSendBuffer(pkt, PKT_S_GAINGOLD); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PING& pkt) { return MakeSendBuffer(pkt, PKT_S_PING); }
	static SendBufferRef MakeSendBuffer(Protocol::S_DUNJEON& pkt) { return MakeSendBuffer(pkt, PKT_S_DUNJEON); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);


#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
#endif
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};