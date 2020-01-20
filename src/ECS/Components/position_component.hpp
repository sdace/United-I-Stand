#pragma once

#include "common.hpp"

struct PositionComponent
{
    PositionComponent(vec2 position)
    {
        Value = position;
    }

    vec2 Value;
};
