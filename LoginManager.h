#pragma once
#include "LoginRoom.h"
class LoginManager
{
public:
    void Init();
    void HandleLogin(GameSessionRef session, const Protocol::C_LOGIN& pkt);

    std::unordered_map<int32, std::weak_ptr<GameSession>> requestMap;
private:
    USE_LOCK;
    //shared_ptr<LoginRoom> _loginRoom;
};

extern LoginManagerRef GLoginManager;