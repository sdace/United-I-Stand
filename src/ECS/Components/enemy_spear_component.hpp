#pragma once

#include "common.hpp"

struct EnemySpearComponent
{
	EnemySpearComponent(vec2 hurtbox)
	{
		Hurtbox = hurtbox;
	}
	vec2 Hurtbox;
};