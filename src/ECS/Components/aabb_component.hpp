#pragma once

#include "common.hpp"

struct AABBComponent
{
    AABBComponent(vec2 offset, vec2 size)
    {
        Offset = offset;
        HalfSize.x = size.x / 2;
        HalfSize.y = size.y / 2;
    }

    vec2 Offset;
    vec2 HalfSize;
};
