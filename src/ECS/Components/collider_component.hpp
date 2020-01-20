#pragma once

#include "ECS/entity.hpp"

struct ColliderComponent
{
    ColliderComponent()
    {
        Platform = Entity::NULL_ENTITY;
    }

    Entity Platform;
};
