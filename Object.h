#pragma once
#include <cmath>
class GameSession;
class Room;
struct Vector3;
enum class ObjectType {
	OBJECT, CREATURE, MONSTER, PLAYER, WARRIOR, MAGE
};

class Object: public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();
	
public:
	Protocol::ObjectInfo* objectInfo;
	Protocol::PosInfo* posInfo;
	Protocol::PosInfo* movePosInfo;
	bool bmovePos = false;					// 도착지 설정이 됐었는지?
	bool IsPlayer(){return _isPlayer;}

	virtual ObjectType GetType() const {return ObjectType::OBJECT;}
	
	float Distance3D(const float X, const float Y, const float Z);
	float Distance2D(const float X, const float Y);
	float Distance2D(const Vector3 src, const Vector3 dest);

    uint64 pingTime = 0;
public:
    std::weak_ptr<Room> room;
    std::weak_ptr<GameSession> session;

protected:
	bool _isPlayer = false;
};

