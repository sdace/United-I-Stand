#include "ECS/Systems/tile_collision_system.hpp"

using namespace std;

void TileCollisionSystem::init()
{
    EntityQuery query =
        EntityQuery().all(4,
                          ComponentType<PositionComponent>::get_id(),
                          ComponentType<VelocityComponent>::get_id(),
                          ComponentType<AABBComponent>::get_id(),
                          ComponentType<TileColliderComponent>::get_id())
					.none(1, ComponentType<EnemySpearComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);

	EntityQuery enemies_spears_query = EntityQuery().all(1, ComponentType<EnemySpearComponent>::get_id());
	m_enemies_spears_query = make_shared<EntityQuery>(move(enemies_spears_query));
	add_query(m_enemies_spears_query);

    tiles = TileManager::get_instance().tile_map;
}

void TileCollisionSystem::update(float elapsed_sec)
{
    auto entities = m_entity_query->get_entities();
    auto m_enemies_spears_entities = m_enemies_spears_query->get_entities();

    for (const auto& e : entities)
    {
        if (!entity_manager->is_valid_entity(e))
        {
            continue;
        }

        VelocityComponent vel = entity_manager->get_component_data<VelocityComponent>(e);
        AABBComponent aabb = entity_manager->get_component_data<AABBComponent>(e);
        PositionComponent pos = entity_manager->get_component_data<PositionComponent>(e);
        bool is_player = entity_manager->has_component<PlayerComponent>(e);

        float centerX = pos.Value.x + aabb.Offset.x;
        float centerY = pos.Value.y + aabb.Offset.y;
        float velocityX = vel.Value.x + vel.External.x;
        float velocityY = vel.Value.y + vel.External.y;

        float dx = CalculateCollision(centerX, centerY, aabb.HalfSize.x, aabb.HalfSize.y, velocityX, 1, is_player);
        if (abs(dx) < abs(velocityX))
        {
            vel.Value.x = dx;
            vel.External.x = 0;
        }
        if (dx < velocityX)
        {
            TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
            tile_collider.Right = true;
            entity_manager->set_component_data(e, move(tile_collider));
        }
        if (dx > velocityX)
        {
            TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
            tile_collider.Left = true;
            entity_manager->set_component_data(e, move(tile_collider));
        }

        pos = entity_manager->get_component_data<PositionComponent>(e);
        centerX = pos.Value.x + aabb.Offset.x;
        centerY = pos.Value.y + aabb.Offset.y;

        float dy = CalculateCollision(centerY, centerX + dx, aabb.HalfSize.y, aabb.HalfSize.x, velocityY, 0, is_player);
        if (abs(dy) < abs(velocityY))
        {
            vel.Value.y = dy;
            vel.External.y = 0;
        }
        if (dy < velocityY)
        {
            TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
            tile_collider.Below = true;
            entity_manager->set_component_data(e, move(tile_collider));
        }
        if (dy > velocityY)
        {
            TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
            tile_collider.Above = true;
            entity_manager->set_component_data(e, move(tile_collider));
        }

        entity_manager->set_component_data(e, move(vel));
    }

	for (const auto& e : m_enemies_spears_entities)
	{
		if (!entity_manager->is_valid_entity(e))
		{
			continue;
		}

		VelocityComponent vel = entity_manager->get_component_data<VelocityComponent>(e);
		AABBComponent aabb = entity_manager->get_component_data<AABBComponent>(e);
		PositionComponent pos = entity_manager->get_component_data<PositionComponent>(e);
		bool is_player = entity_manager->has_component<PlayerComponent>(e);

		float centerX = pos.Value.x + aabb.Offset.x;
		float centerY = pos.Value.y + aabb.Offset.y;
		float velocityX = vel.Value.x + vel.External.x;
		float velocityY = vel.Value.y + vel.External.y;

		float dx = CalculateCollision(centerX, centerY, aabb.HalfSize.x, aabb.HalfSize.y, velocityX, 1, is_player);
		if (abs(dx) < abs(velocityX))
		{
			vel.Value.x = dx;
			vel.External.x = 0;
		}
		if (dx < velocityX)
		{
			TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
			tile_collider.Right = true;	
			entity_manager->set_component_data(e, move(tile_collider));
		}
		if (dx > velocityX)
		{
			TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
			tile_collider.Left = true;
			entity_manager->set_component_data(e, move(tile_collider));
		}

		pos = entity_manager->get_component_data<PositionComponent>(e);
		centerX = pos.Value.x + aabb.Offset.x;
		centerY = pos.Value.y + aabb.Offset.y;

		float dy = CalculateCollision(centerY, centerX + dx, aabb.HalfSize.y, aabb.HalfSize.x, velocityY, 0, is_player);
		if (abs(dy) < abs(velocityY))
		{
			vel.Value.y = dy;
			vel.External.y = 0;
		}
		if (dy < velocityY)
		{
			TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
			tile_collider.Below = true;
			entity_manager->set_component_data(e, move(tile_collider));
		}
		if (dy > velocityY)
		{
			TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
			tile_collider.Above = true;
			entity_manager->destroy_entity(e);
			entity_manager->set_component_data(e, move(tile_collider));
		}
		TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
		if (tile_collider.Above || tile_collider.Below || tile_collider.Left || tile_collider.Right) {
			entity_manager->destroy_entity(e);
		}
		else {
			entity_manager->set_component_data(e, move(vel));
		}
	}
}



// CalculateCollision is based on the algorithm described by the website below
//http://higherorderfun.com/blog/2012/05/20/the-guide-to-implementing-2d-platformers/
float TileCollisionSystem::CalculateCollision(float centerA, float centerB, float halfSizeA, float halfSizeB, float velocity, int axis, bool is_player)
{
    centerA = centerA / TILE_SIZE;
    centerB = centerB / TILE_SIZE;
    halfSizeA = halfSizeA / TILE_SIZE;
    halfSizeB = halfSizeB / TILE_SIZE;
    velocity = velocity / TILE_SIZE;

    int direction = (velocity > 0) ? 1 : -1;
    if (velocity == 0) return 0;

    float forwardEdge = centerA + (halfSizeA * direction);
    int tileCollisionOffset = (direction == 1) ? 0 : 1;
    int colliderMax = (int)ceil(centerB + halfSizeB - EPSILON) - 1;
    int colliderMin = (int)floor(centerB - halfSizeB + EPSILON);
    int colliderRow = (direction == 1) ? (int)ceil(forwardEdge - EPSILON) - 1 : (int)floor(forwardEdge + EPSILON);

    float distanceToNearestTile = (direction == 1) ? 1000 : -1000;
    bool nearest_tile_is_spike = false;
    int tilesToCheck = (int)ceil(abs(velocity)) + 1;
    for (int i = colliderMin; i <= colliderMax; i++)
    {
        for (int j = 0; j < tilesToCheck; j++)
        {
            int currentTile = colliderRow + direction + (j * direction);

            int solidTile = (axis == 1) ?
                ContainsTile(currentTile, i) :
                ContainsTile(i, currentTile);
            if (solidTile)
            {
                float distanceToTile = (currentTile + tileCollisionOffset) - forwardEdge;
                if (direction == 1)
                {
                    if (distanceToTile < distanceToNearestTile)
                    {
                        if (TouchingSpike(direction, axis, currentTile, i))
                        {
                            nearest_tile_is_spike = true;
                        }
                        else
                        {
                            nearest_tile_is_spike = false;
                        }
                    }
                    distanceToNearestTile = min(distanceToNearestTile, distanceToTile);
                }
                else
                {
                    if (distanceToTile > distanceToNearestTile)
                    {
                        if (TouchingSpike(direction, axis, currentTile, i))
                        {
                            nearest_tile_is_spike = true;
                        }
                        else
                        {
                            nearest_tile_is_spike = false;
                        }
                    }
                    distanceToNearestTile = max(distanceToNearestTile, distanceToTile);
                }
            }
        }
    }

    float movement;
    if (direction == 1)
    {
        movement = min(velocity, distanceToNearestTile);
        if (movement < velocity && nearest_tile_is_spike && is_player)
        {
            event_manager->post(PlayerDeathEvent());
            return 0.0f;
        }
    }
    else
    {
        movement = max(velocity, distanceToNearestTile);
        if (movement > velocity && nearest_tile_is_spike && is_player)
        {
            event_manager->post(PlayerDeathEvent());
            return 0.0f;
        }
    }

    return movement * TILE_SIZE;
}

bool TileCollisionSystem::ContainsTile(int x, int y)
{
    return TileManager::get_instance().tile_map.is_attribute("solid", y, x);
}

bool TileCollisionSystem::TouchingSpike(int direction, int axis, int x, int y)
{
    if (axis == 0)
    {
        if (direction == 1)
        {
            return TileManager::get_instance().tile_map.is_attribute("spike_top", x, y);
        }
        else
        {
            return TileManager::get_instance().tile_map.is_attribute("spike_bottom", x, y);
        }
    }
    else
    {
        if (direction == 1)
        {
            return TileManager::get_instance().tile_map.is_attribute("spike_left", y, x);
        }
        else
        {
            return TileManager::get_instance().tile_map.is_attribute("spike_right", y, x);
        }
    }
}

void ResetGroundedSystem::init()
{
    EntityQuery query =
        EntityQuery().all(1,
                          ComponentType<TileColliderComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);
}

void ResetGroundedSystem::update(float elapsed_time)
{
    auto entities = m_entity_query->get_entities();

    for (const auto& e : entities)
    {
        TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
        tile_collider.Below = false;
        tile_collider.Above = false;
        tile_collider.Right = false;
        tile_collider.Left = false;
        entity_manager->set_component_data(e, move(tile_collider));
    }
}
