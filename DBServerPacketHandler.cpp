#include "pch.h"
#include "DBServerPacketHandler.h"
#include "CharacterDB.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"
#include "LoginManager.h"
PacketHandlerFunc GDBPacketHandler[UINT16_MAX];
bool Handle_S_DB_GAINEXP(PacketSessionRef& session, Protocol::S_DB_GAINEXP& pkt) {

	return true;
}
bool Handle_S_DB_GAINGOLD(PacketSessionRef& session, Protocol::S_DB_GAINGOLD& pkt) {

	return true;
}
bool Handle_S_DB_LOADEQUIPMENT(PacketSessionRef& session, Protocol::S_DB_LOADEQUIPMENT& pkt)
{
    //LOG("LOAD EQU");
    auto it = CharacterDB::g_requestMap.find(pkt.requestid());
    if (it != CharacterDB::g_requestMap.end())
    {
        GameSessionRef session = it->second.lock();
        if (session)
        {
            session->player->Equipment->LoadEquipment(pkt);

            // 장비 옵션적용 나중에 함수화
            session->player->objectInfo->set_maxhp(session->player->GetPlayerMaxHp());
            int curHp = std::min<int>(session->player->objectInfo->hp(), session->player->GetPlayerMaxHp());
            session->player->objectInfo->set_hp(curHp);


            Protocol::S_LOADEQUIPMENT sendPkt;
            Protocol::ObjectInfo* object = sendPkt.mutable_objectinfo();
            object->CopyFrom(*session->player->objectInfo);
            for (auto item : pkt.items()) {
                Protocol::ItemData* myItem = sendPkt.add_items();
                myItem->CopyFrom(item);
            }

            // 클라이언트에게 장착중인 장비 전송
            SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(sendPkt);
            if (auto Room = session->player->room.lock()) {
                Room->Broadcast(sendBuffer);
            }
        }

        // 반드시 지우기
        CharacterDB::g_requestMap.erase(it);
    }
    else
    {
        // 잘못된 requestId 응답 처리
        return false;
    }

    return true;
}
bool Handle_S_DB_LOADINVENTORY(PacketSessionRef& session, Protocol::S_DB_LOADINVENTORY& pkt)
{
    //LOG("LOAD INVEN");
    auto it = CharacterDB::g_requestMap.find(pkt.requestid());
    if (it != CharacterDB::g_requestMap.end())
    {
        GameSessionRef session = it->second.lock();
        if (session)
        {
            // 플레이어 인벤토리에 데이터 로드
            session->player->Inventory->LoadInventory(pkt);

            Protocol::S_LOADINVENTORY sendPkt;
            for (auto item : pkt.items()) {
                Protocol::ItemData* myItem = sendPkt.add_items();
                myItem->CopyFrom(item);
            }

            // 클라이언트에게 플레이어 인벤토리 전달
            SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(sendPkt);
            session->Send(sendBuffer);
        }

        // 반드시 지우기
        CharacterDB::g_requestMap.erase(it);
    }
    else
    {
        // 잘못된 requestId 응답 처리
        return false;
    }

    return true;
}

bool Handle_S_DB_DELETEINVENTORY(PacketSessionRef& session, Protocol::S_DB_DELETEINVENTORY& pkt)
{
    return false;
}

bool Handle_S_DB_UPDATEINVENTORY(PacketSessionRef& session, Protocol::S_DB_UPDATEINVENTORY& pkt)
{
    return false;
}
bool Handle_S_DB_INSERTINVENTORY(PacketSessionRef& session, Protocol::S_DB_INSERTINVENTORY& pkt)
{
    return false;
}
bool Handle_S_DB_EQUIPMENT(PacketSessionRef& session, Protocol::S_DB_EQUIPMENT& pkt)
{
    return false;
}

bool Handle_S_DB_PING(PacketSessionRef& session, Protocol::S_DB_PING& pkt) {


	return true;
}

bool Handle_S_DB_UPDATEPOS(PacketSessionRef& session, Protocol::S_DB_UPDATEPOS& pkt) {

    return true;
}

bool Handle_S_DB_LOGIN(PacketSessionRef& session, Protocol::S_DB_LOGIN& pkt)
{
    // TODO 받아온 데이터를 다시 S_LOGIN으로 보내줌.
    auto it = GLoginManager->requestMap.find(pkt.requestid());
    if (it != GLoginManager->requestMap.end())
    {
        GameSessionRef session = it->second.lock();
        if (session)
        {
            session->SetDBID(pkt.dbid());
            Protocol::S_LOGIN sendPkt;
            sendPkt.set_success(pkt.success());
            sendPkt.set_errortype(pkt.errortype());
            
            for (auto& info : pkt.objectinfos()) {
                Protocol::ObjectInfo* characterData = sendPkt.add_objectinfos();
                characterData->CopyFrom(info);
            }

            // 클라이언트에게 플레이어 인벤토리 전달
            SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(sendPkt);
            session->Send(sendBuffer);
        }

        // 반드시 지우기
        GLoginManager->requestMap.erase(it);
    }
    else
    {
        // 잘못된 requestId 응답 처리
        return false;
    }

   
    
    
    return true;
}