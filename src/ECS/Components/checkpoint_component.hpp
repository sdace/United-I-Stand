#pragma once

struct CheckpointComponent
{
    CheckpointComponent(int id) : id(id) 
    {
        Activated = false;
    }
    int id;
    bool Activated;
};
