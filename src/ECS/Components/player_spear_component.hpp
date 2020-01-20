#pragma once

#include "common.hpp"

struct PlayerSpearComponent
{
	PlayerSpearComponent(vec2 hurtbox)
	{
		Hurtbox = hurtbox;
	}
	vec2 Hurtbox;
};