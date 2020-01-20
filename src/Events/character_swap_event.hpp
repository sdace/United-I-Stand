#pragma once

#include "ECS/Components/player_component.hpp"

class CharacterSwapEvent
{
public:
    CharacterSwapEvent(Player new_character) : new_character(new_character) { }
    Player new_character;
};
