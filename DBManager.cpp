#include "pch.h"
#include "DBManager.h"
#include "DBSession.h"

DBManager GDBManager;
void DBManager::Add(DBSessionRef session)
{
	WRITE_LOCK;
	_sessions.insert(session);

}

void DBManager::Remove(DBSessionRef session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void DBManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (DBSessionRef session : _sessions)
	{
		session->Send(sendBuffer);
	}
}