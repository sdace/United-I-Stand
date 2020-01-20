#pragma once

#include "ECS/entity.hpp"

struct EnemyComponent
{
	EnemyComponent(bool trace_player, float trace_speed, bool shoot_spear) 
	{
		Trace_Player = trace_player;
		Trace_Speed = trace_speed;
		Shoot_Spear = shoot_spear;
	}
	bool Trace_Player;
	float Trace_Speed;
	bool Shoot_Spear;
	Entity Spear;
	vec2 Value;
	// jump_frame is for time between each jump while tracing enemies
	float jump_frame = 0.f;
	// jump_finished_frame is for how long the enemy will finish a jump
	float jump_finished_frame = -1.f;
	// is_jump_up is true when enemy is jumping up, is false when enemy finished jumping and is falling
	bool is_jump_up = true;
	// Spear is for enemy to throw spear
	Entity enemy_spear;
	// how many frames for throwing one spear
	int throw_spear_frame = 119;
	int animation_frame = 30;
};