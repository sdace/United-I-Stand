#pragma once

#include "ECS/entity_manager.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/tile_collider_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/facing_component.hpp"
#include "ECS/Components/gravity_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/death_component.hpp"
#include "Events/player_death_event.hpp"
#include "TI/tilemap.hpp"
#include "TI/tile_importer.hpp"
#include "ECS/Components/enemy_spear_component.hpp"


class TileCollisionSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;
    float CalculateCollision(float centerA, float centerB, float halfSizeA, float halfSizeB, float velocity, int axis, bool is_player);
    bool ContainsTile(int x, int y);
    bool TouchingSpike(int direction, int axis, int x, int y);
    TileMap tiles;
	shared_ptr<EntityQuery> m_enemies_spears_query;
};

class ResetGroundedSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;
};
