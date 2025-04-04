#include "pch.h"
#include "PlayerLevel.h"
#include <unordered_map>
#include "Player.h"
#include "CharacterDB.h"
#include "ClientPacketHandler.h"
#include "Room.h"
std::unordered_map<int, int> PlayerLevel::LevelExpTable;
int PlayerLevel::MaxLevel;
PlayerLevel::PlayerLevel()
{
}

PlayerLevel::~PlayerLevel()
{
}

void PlayerLevel::Init()
{
    MaxLevel = 50;
    LevelExpTable[0] = 1000;
    for (int i = 1; i < MaxLevel; i++) {
        LevelExpTable[i] = LevelExpTable[i-1] * 1.2;
    }
}

void PlayerLevel::AddExp(PlayerRef player, int gainedExp)
{
    GameSessionRef session = player->session.lock();
    if(session == nullptr) return;
    uint64 db_userid = session->myCharacterDBID[player->objectInfo->object_id()];
    int PlayerLevel = player->objectInfo->playerlevel();
    int PlayerCurExp = player->objectInfo->curexp();
    int AddExp = PlayerCurExp + gainedExp;      // �÷��̾� ����ġ�� ��ģ��
    int RemainExp = AddExp;

    if (PlayerLevel >= MaxLevel) {
        // �ִ뷹���� ����ġ�� ��������.
        return;
    }
    
    
    if (AddExp >= LevelExpTable[PlayerLevel])
    {
        // TODO ������ DB����
        RemainExp = AddExp - LevelExpTable[PlayerLevel]; //�������ϰ� ���� ����ġ
        PlayerLevel++;  //������
        
        CharacterDB::UpdatePlayerLevelUp( db_userid, PlayerLevel, RemainExp);
        player->objectInfo->set_totalexp(LevelExpTable[PlayerLevel]);
        player->objectInfo->set_maxhp(PlayerLevel * player->GetAddLevelUpHP());

        player->objectInfo->set_playerlevel(PlayerLevel);
        player->objectInfo->set_curexp(RemainExp);


        Protocol::S_GAINEXP pkt;
        Protocol::ObjectInfo* objectinfo = pkt.mutable_objectinfo();
        objectinfo->CopyFrom(*player->objectInfo);
        auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        RoomRef room = player->room.lock();
        if (room != nullptr)
            room->Broadcast(sendBuffer);

    }
    else {
        player->objectInfo->set_playerlevel(PlayerLevel);
        player->objectInfo->set_curexp(RemainExp);
        CharacterDB::UpdatePlayerExp( db_userid, RemainExp);

        player->objectInfo->set_playerlevel(PlayerLevel);
        player->objectInfo->set_curexp(RemainExp);


        Protocol::S_GAINEXP pkt;
        Protocol::ObjectInfo* objectinfo = pkt.mutable_objectinfo();
        objectinfo->CopyFrom(*player->objectInfo);
        auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        if (session != nullptr)
            session->Send(sendBuffer);

    }
    
    

    
}
