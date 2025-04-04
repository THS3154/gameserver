#pragma once
#include "Object.h"
#include "MonsterInfo.h"
class Creature :public Object
{

public:
	Creature();
	virtual ~Creature();
	uint64 GetMyChanel(){return MyChanel;}
	void SetMyChanel(uint64 chanel){
		MyChanel = chanel;
		objectInfo->set_chanel(chanel);
	}
	virtual ObjectType GetType() const override { return ObjectType::CREATURE; }
public:
	StatInfo stat;


	
public:
	int SpawnerID;
	uint64 MyChanel;
};

