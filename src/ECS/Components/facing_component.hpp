#pragma once

enum FacingDirection
{
    RIGHT = 1,
    LEFT = -1
};

struct FacingComponent
{
    FacingComponent(FacingDirection direction)
    {
        Value = direction;
    }

    FacingDirection Value;
};
