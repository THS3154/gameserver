#pragma once
#include "Room.h"
class LoginRoom :
    public Room
{

public:
    void Enter(GameSessionRef session); // 접속했을때
    void HandleLogin(GameSessionRef session, const Protocol::C_LOGIN& pkt);
};

