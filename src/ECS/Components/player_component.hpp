#pragma once

#include "ECS/entity.hpp"

enum Player
{
    BUNNY,
    SPARTAN,
    WHEEL
};

struct PlayerComponent
{
    Player State = BUNNY;
    Entity Spear;
    bool boosted = 0;
};
