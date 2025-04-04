#include "pch.h"
#include "PlayerGold.h"
#include "Player.h"
#include "characterDB.h"

void PlayerGold::UpdateGold(PlayerRef player, int gainedGold, LOG_GOLD logs)
{
    GameSessionRef session = player->session.lock();
    if (session == nullptr) return;// MESSAGE_GOLD_NONEPLAYER;

    uint64 db_userid = session->myCharacterDBID[player->objectInfo->object_id()];
    const int myGold = player->objectInfo->gold();      // ������ ���
    int currentGold = myGold + gainedGold;              // ���� ������Ʈ �� ���
    if (currentGold < 0) {
        return;// MESSAGE_GOLD_LACK; //������
    }
    
    // TODO ����ó��
    CharacterDB::UpdatePlayerGold(db_userid, gainedGold, logs); //DB�� ������Ʈ
    player->objectInfo->set_gold(currentGold);                  // ������ �����ϴ� �÷��̾� ��尪 ������Ʈ

    Protocol::S_GAINGOLD pkt;
    Protocol::ObjectInfo* objectinfo = pkt.mutable_objectinfo();
    objectinfo->CopyFrom(*player->objectInfo);

    auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
    session->Send(sendBuffer);
    
    return;// MESSAGE_GOLD_SUCCESS;
}
