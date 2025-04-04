#pragma once
#include "Protocol.pb.h"
#include "DBProtocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "lllllllllqqq.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GDBPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_DB_LOGIN = 1000,
	PKT_S_DB_LOGIN = 1001,
	PKT_C_DB_GAINEXP = 1002,
	PKT_S_DB_GAINEXP = 1003,
	PKT_C_DB_GAINGOLD = 1004,
	PKT_S_DB_GAINGOLD = 1005,
	PKT_C_DB_LOADEQUIPMENT = 1006,
	PKT_S_DB_LOADEQUIPMENT = 1007,
	PKT_C_DB_LOADINVENTORY = 1008,
	PKT_S_DB_LOADINVENTORY = 1009,
	PKT_C_DB_DELETEINVENTORY = 1010,
	PKT_S_DB_DELETEINVENTORY = 1011,
	PKT_C_DB_UPDATEINVENTORY = 1012,
	PKT_S_DB_UPDATEINVENTORY = 1013,
	PKT_C_DB_INSERTINVENTORY = 1014,
	PKT_S_DB_INSERTINVENTORY = 1015,
	PKT_C_DB_EQUIPMENT = 1016,
	PKT_S_DB_EQUIPMENT = 1017,
	PKT_C_DB_UPDATEPOS = 1018,
	PKT_S_DB_UPDATEPOS = 1019,
	PKT_C_DB_PING = 1020,
	PKT_S_DB_PING = 1021,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_DB_LOGIN(PacketSessionRef& session, Protocol::S_DB_LOGIN& pkt);
bool Handle_S_DB_GAINEXP(PacketSessionRef& session, Protocol::S_DB_GAINEXP& pkt);
bool Handle_S_DB_GAINGOLD(PacketSessionRef& session, Protocol::S_DB_GAINGOLD& pkt);
bool Handle_S_DB_LOADEQUIPMENT(PacketSessionRef& session, Protocol::S_DB_LOADEQUIPMENT& pkt);
bool Handle_S_DB_LOADINVENTORY(PacketSessionRef& session, Protocol::S_DB_LOADINVENTORY& pkt);
bool Handle_S_DB_DELETEINVENTORY(PacketSessionRef& session, Protocol::S_DB_DELETEINVENTORY& pkt);
bool Handle_S_DB_UPDATEINVENTORY(PacketSessionRef& session, Protocol::S_DB_UPDATEINVENTORY& pkt);
bool Handle_S_DB_INSERTINVENTORY(PacketSessionRef& session, Protocol::S_DB_INSERTINVENTORY& pkt);
bool Handle_S_DB_EQUIPMENT(PacketSessionRef& session, Protocol::S_DB_EQUIPMENT& pkt);
bool Handle_S_DB_UPDATEPOS(PacketSessionRef& session, Protocol::S_DB_UPDATEPOS& pkt);
bool Handle_S_DB_PING(PacketSessionRef& session, Protocol::S_DB_PING& pkt);

class DBServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GDBPacketHandler[i] = Handle_INVALID;
		GDBPacketHandler[PKT_S_DB_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_LOGIN>(Handle_S_DB_LOGIN, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_GAINEXP] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_GAINEXP>(Handle_S_DB_GAINEXP, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_GAINGOLD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_GAINGOLD>(Handle_S_DB_GAINGOLD, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_LOADEQUIPMENT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_LOADEQUIPMENT>(Handle_S_DB_LOADEQUIPMENT, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_LOADINVENTORY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_LOADINVENTORY>(Handle_S_DB_LOADINVENTORY, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_DELETEINVENTORY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_DELETEINVENTORY>(Handle_S_DB_DELETEINVENTORY, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_UPDATEINVENTORY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_UPDATEINVENTORY>(Handle_S_DB_UPDATEINVENTORY, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_INSERTINVENTORY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_INSERTINVENTORY>(Handle_S_DB_INSERTINVENTORY, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_EQUIPMENT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_EQUIPMENT>(Handle_S_DB_EQUIPMENT, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_UPDATEPOS] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_UPDATEPOS>(Handle_S_DB_UPDATEPOS, session, buffer, len); };
		GDBPacketHandler[PKT_S_DB_PING] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DB_PING>(Handle_S_DB_PING, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GDBPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_GAINEXP& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_GAINEXP); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_GAINGOLD& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_GAINGOLD); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_LOADEQUIPMENT& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_LOADEQUIPMENT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_LOADINVENTORY& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_LOADINVENTORY); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_DELETEINVENTORY& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_DELETEINVENTORY); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_UPDATEINVENTORY& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_UPDATEINVENTORY); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_INSERTINVENTORY& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_INSERTINVENTORY); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_EQUIPMENT& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_EQUIPMENT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_UPDATEPOS& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_UPDATEPOS); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DB_PING& pkt) { return MakeSendBuffer(pkt, PKT_C_DB_PING); }

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