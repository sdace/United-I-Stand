#pragma once

struct TileColliderComponent 
{
    TileColliderComponent()
    {
        Below = false;
        Above = false;
        Right = false;
        Left = false;
    }

    bool Below;
    bool Above;
    bool Right;
    bool Left;
};
