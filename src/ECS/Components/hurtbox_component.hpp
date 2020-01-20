#pragma once

#include "common.hpp"

struct HurtboxComponent
{
	HurtboxComponent(vec2 hurtbox)
	{
		Hurtbox = hurtbox;
	}
	vec2 Hurtbox;
};