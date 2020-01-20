#pragma once

#include "common.hpp"

struct KillableComponent 
{
	KillableComponent(bool killable)
	{
		Killable = killable;
	}
	bool Killable;
};