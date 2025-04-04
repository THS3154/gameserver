#pragma once
#include "JobQueue.h"
enum LOG_GOLD
{
	LOG_GOLD_GET_MONSTER = 1,	// ���͸� ������� ��带 ����
	LOG_GOLD_GET_STORE = 2,		// ������ �Ǹ������� ��带 ����
	LOG_GOLD_SPEND_STORE = 3,	// �������� ���� ������ ��带 �Ҹ�
};

enum MESSAGE_GOLD {
	MESSAGE_GOLD_SUCCESS = 1,	// ����
	MESSAGE_GOLD_LACK = 2,		// ������ �ŷ� �Ұ�(��κ� �����ʿ��� ó���ɵ�)
	
	
	
	MESSAGE_GOLD_NONEPLAYER = 999,	// �÷��̾� �����Ͱ� ���������� ������ �ʾ�����
};
class PlayerGold : public JobQueue
{
public:
	static void UpdateGold(PlayerRef player, int gainedGold, LOG_GOLD logs);
};

