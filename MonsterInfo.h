#pragma once
#include <vector>

struct MonsterDropInfo
{
    int code;
    float rate;
};

struct StatInfo
{
	string name;
	float max_hp;		// ü��
	float max_mp;		// ����
	float defalut_speed;// �⺻ �ӵ�
	float cur_hp;		// ���� ü��
	float cur_mp;		// ���� ����
	float cur_speed;	// ���� �ӵ�
	float shield;		// ����

	float searchradius;	// ��׷ι���
	int regentime;	// ���� �ֱ� ����ms
	int exp;
	int gold;

    std::vector<MonsterDropInfo> dropTable;
};

struct SpawnLocation
{
	float x;
	float y;
	float z;
	float yaw;
};