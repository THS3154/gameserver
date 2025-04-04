#pragma once
#include "Room.h"

class BaseLevel :
    public Room
{
public:
    
    BaseLevel();
    ~BaseLevel();

    void Init() override;
    void UpdateTick() override;

    


};



