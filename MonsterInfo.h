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
	float max_hp;		// 체력
	float max_mp;		// 마나
	float defalut_speed;// 기본 속도
	float cur_hp;		// 현재 체력
	float cur_mp;		// 현재 마나
	float cur_speed;	// 현재 속도
	float shield;		// 쉴드

	float searchradius;	// 어그로범위
	int regentime;	// 리젠 주기 단위ms
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