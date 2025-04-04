#pragma once
#include "Session.h"
#include <mutex>
class Player;

class GameSession : public PacketSession
{
public:
	~GameSession()
	{
		//cout << "~GameSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	void SetDBID(uint64 id) { DBID = id; }
	uint64 GetDBID() { return DBID; }
public:
	std::shared_ptr<Player> player;
	std::mutex playerMutex;
	
	unordered_map<uint64, uint64> myCharacterDBID;
	//atomic<weak_ptr<Player>> player;

private:
	uint64 DBID;
};