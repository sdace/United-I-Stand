#pragma once

struct SolidComponent 
{
    SolidComponent(bool solidTop)
    {
        SolidTop = solidTop;
    }

    bool SolidTop;
};
