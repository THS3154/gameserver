#include "pch.h"
#include "LoginManager.h"
#include "ObjectUtils.h"
#include "DBServerPacketHandler.h"
#include "DBManager.h"
LoginManagerRef GLoginManager;
void LoginManager::Init()
{
    //_loginRoom = MakeShared<LoginRoom>();
}

void LoginManager::HandleLogin(GameSessionRef session, const Protocol::C_LOGIN& pkt)
{
    int64 DBGenerator = ObjectUtils::s_DBGenerator.fetch_add(1);
    
    WRITE_LOCK;
    requestMap[DBGenerator] = session;

    //TODO 패킷보냄
    Protocol::C_DB_LOGIN sendPkt;
    sendPkt.set_id(pkt.id());
    sendPkt.set_pw(pkt.pw());
    sendPkt.set_requestid(DBGenerator);

    SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(sendPkt);
    GDBManager.Broadcast(sendBuffer);

    // TODO -> DBServerPacketHandler -> S_DB_LOGIN함수로
    // TODO -> DB서버 -> C_DB_LOGIN
}
