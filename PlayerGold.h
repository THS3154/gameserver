#pragma once
#include "JobQueue.h"
enum LOG_GOLD
{
	LOG_GOLD_GET_MONSTER = 1,	// 몬스터를 잡았을때 골드를 얻음
	LOG_GOLD_GET_STORE = 2,		// 상점에 판매했을때 골드를 얻음
	LOG_GOLD_SPEND_STORE = 3,	// 상점에서 구매 했을때 골드를 소모
};

enum MESSAGE_GOLD {
	MESSAGE_GOLD_SUCCESS = 1,	// 성공
	MESSAGE_GOLD_LACK = 2,		// 골드부족 거래 불가(대부분 상점쪽에서 처리될듯)
	
	
	
	MESSAGE_GOLD_NONEPLAYER = 999,	// 플레이어 데이터가 정상적으로 들어오지 않았을때
};
class PlayerGold : public JobQueue
{
public:
	static void UpdateGold(PlayerRef player, int gainedGold, LOG_GOLD logs);
};

