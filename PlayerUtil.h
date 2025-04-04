#pragma once
#include "PublicUtil.h"

/*******************************************
			플레이어 기능
*******************************************/



class PlayerUtil : virtual public PublicUtil
{
public:
	PlayerUtil();
	virtual ~PlayerUtil();

	std::vector<uint64> GetNearbyMonsters(float playerX, float playerY, int searchRange);
protected:
	void AddPlayerToGrid(uint64 playerID, float x, float y);
	void RemovePlayerToGrid(uint64 playerID, float x, float y);

};




