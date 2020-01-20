#pragma once
#include "common.hpp"
#include <string>
struct RenderableComponent
{
    RenderableComponent(std::string gstate)
        :gstate(gstate) {
    };

    std::string gstate;
    Transform transform;
};