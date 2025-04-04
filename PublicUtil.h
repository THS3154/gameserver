#pragma once
#include <unordered_map>

struct pair_hash {
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& p) const {
		return std::hash<T1>()(p.first) ^ (std::hash<T2>()(p.second) << 1);
	}

};

/**************************************************************************************
					�÷��̾�� ���Ͱ� �������� �������� �͵�
**************************************************************************************/
class PublicUtil 
{
public:
	virtual ~PublicUtil();
protected:
	unordered_map<std::pair<int, int>, std::vector<uint64>, pair_hash> GridPlayers;
	unordered_map<std::pair<int, int>, std::vector<uint64>, pair_hash> GridMonsters;

protected:
	// �׸��� �÷��̾� ����
	const int GRID_SIZE = 500;
};

