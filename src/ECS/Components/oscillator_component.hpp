#pragma once
#include "common.hpp"

struct OscillatorComponent {

    OscillatorComponent(vec2 origin, vec2 displacement, int speed, bool circular)
    {
        Origin = origin;
        Displacement = displacement;
        Speed = speed;
        Circular = circular;
    }

    float Counter = 0;
    vec2 Origin;
    vec2 Displacement;
    int Speed;
    bool Circular;
};
