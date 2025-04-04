#include "pch.h"
#include "PlayerGold.h"
#include "Player.h"
#include "characterDB.h"

void PlayerGold::UpdateGold(PlayerRef player, int gainedGold, LOG_GOLD logs)
{
    GameSessionRef session = player->session.lock();
    if (session == nullptr) return;// MESSAGE_GOLD_NONEPLAYER;

    uint64 db_userid = session->myCharacterDBID[player->objectInfo->object_id()];
    const int myGold = player->objectInfo->gold();      // 내현재 골드
    int currentGold = myGold + gainedGold;              // 최종 업데이트 될 골드
    if (currentGold < 0) {
        return;// MESSAGE_GOLD_LACK; //골드부족
    }
    
    // TODO 정상처리
    CharacterDB::UpdatePlayerGold(db_userid, gainedGold, logs); //DB에 업데이트
    player->objectInfo->set_gold(currentGold);                  // 서버에 존재하는 플레이어 골드값 업데이트

    Protocol::S_GAINGOLD pkt;
    Protocol::ObjectInfo* objectinfo = pkt.mutable_objectinfo();
    objectinfo->CopyFrom(*player->objectInfo);

    auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
    session->Send(sendBuffer);
    
    return;// MESSAGE_GOLD_SUCCESS;
}
