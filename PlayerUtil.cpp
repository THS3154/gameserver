#include "pch.h"
#include "PlayerUtil.h"



PlayerUtil::PlayerUtil()
{
}

PlayerUtil::~PlayerUtil()
{
}

void PlayerUtil::AddPlayerToGrid(uint64 playerID, float x, float y)
{
	int gridX = x / GRID_SIZE;
	int gridY = y / GRID_SIZE;
	GridPlayers[{gridX, gridY}].push_back(playerID);
}

void PlayerUtil::RemovePlayerToGrid(uint64 playerID, float x, float y)
{
	int gridX = x / GRID_SIZE;
	int gridY = y / GRID_SIZE;

	auto& players = GridPlayers[{gridX, gridY}];
	players.erase(std::remove(players.begin(), players.end(), playerID), players.end());
}


std::vector<uint64> PlayerUtil::GetNearbyMonsters(float playerX, float playerY, int searchRange)
{
	int gridX = playerX / GRID_SIZE;
	int gridY = playerY / GRID_SIZE;

	std::vector<uint64_t> nearbyPlayers;
	for (int dx = -searchRange; dx <= searchRange; dx++) {
		for (int dy = -searchRange; dy <= searchRange; dy++) {
			auto it = GridMonsters.find({ gridX + dx, gridY + dy });
			if (it != GridMonsters.end()) {
				nearbyPlayers.insert(nearbyPlayers.end(), it->second.begin(), it->second.end());
			}
		}
	}
	return nearbyPlayers;
}