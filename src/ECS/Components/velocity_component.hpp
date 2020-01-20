#pragma once

#include "common.hpp"
#include <algorithm>

using namespace std;

struct VelocityComponent
{
    VelocityComponent(vec2 velocity, vec2 max_velocity)
    {
        Value = velocity;
        Previous = velocity;
        External = { 0.0f, 0.0f };
        Max = max_velocity;
    }

    void clamp()
    {
        Value.x = min(Max.x, max(-Max.x, Value.x));
        Value.y = min(Max.y, max(-Max.y, Value.y));
    }

	vec2 Value;
    vec2 External;
    vec2 Previous;
    vec2 Max;
};
