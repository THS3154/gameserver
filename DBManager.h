#pragma once

class DBSession;

using DBSessionRef = shared_ptr<DBSession>;

class DBManager
{
public:
	void Add(DBSessionRef session);
	void Remove(DBSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	Set<DBSessionRef> _sessions;
};

extern DBManager GDBManager;
