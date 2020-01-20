#pragma once

struct Room
{
    int id;
    float x_max;
    float x_min;
    float y_max;
    float y_min;

    friend bool operator==(const Room& lhs, const Room& rhs)
    {
        return (lhs.id == rhs.id);
    }
};