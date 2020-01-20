#pragma once

// internal
#include "ECS/entity.hpp"
#include "ECS/component_type.hpp"

// stlib
#include <vector>
#include <unordered_set>
#include <bitset>

using namespace std;

class EntityQuery
{
public:
    EntityQuery();
    EntityQuery all(int n, ...);
    EntityQuery none(int n, ...);
    EntityQuery one(int n, ...);

    unordered_set<Entity> get_entities();

    void update_query(Entity e, ComponentSet component_set);

private:
    bool m_is_match(ComponentSet component_set);

    ComponentSet m_all;
    ComponentSet m_none;
    ComponentSet m_one;

    unordered_set<Entity> m_entities;
};
