#pragma once
#include "DBServerPacketHandler.h"
#include "DBManager.h"
class DBSession :
    public PacketSession
{

public:
	~DBSession()
	{
		cout << "~DBSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << u8"DB ���� ���� ����" << endl;
		GDBManager.Add(static_pointer_cast<DBSession>(shared_from_this()));
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		// TODO : packetId �뿪 üũ
		DBServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		GDBManager.Remove(static_pointer_cast<DBSession>(shared_from_this()));
		//cout << "Disconnected" << endl;
	}

};

