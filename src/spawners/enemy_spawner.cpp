#include "enemy_spawner.hpp"

RoomService& rs = RoomService::get_instance();
void EnemySpawner::init(shared_ptr<EntityManager> entityManager, shared_ptr<EventManager> event_manager)
{
    using namespace std;
    entity_manager = entityManager;
    EntityQuery player_query = EntityQuery().all(1, ComponentType<PlayerComponent>::get_id());
    EntityQuery enemies_query = EntityQuery().all(1, ComponentType<EnemyComponent>::get_id());
    EntityQuery bullet_query = EntityQuery().all(1, ComponentType<EnemySpearComponent>::get_id());
    m_player_query = make_shared<EntityQuery>(move(player_query));
    m_enemies_query = make_shared<EntityQuery>(move(enemies_query));
    m_bullet_query = make_shared<EntityQuery>(move(bullet_query));
    entity_manager->register_query(m_player_query);
    entity_manager->register_query(m_enemies_query);
    entity_manager->register_query(m_bullet_query);

    auto respawn_handler = bind(&EnemySpawner::player_respawn_handler, this, placeholders::_1);
    event_manager->add_handler<PlayerRespawnEvent>(respawn_handler);

    pugi::xml_parse_result result = doc.load_file(PATH_TMX);
    if (!result)
    {
        cout << "fail to load the tmx file!" << endl;
        return;
    }

    auto object_groups = doc.child("map").children("objectgroup");
    pugi::xml_node enemies;
    for (auto& og : object_groups)
    {
        auto name = og.attribute("name");
        if (strcmp(name.as_string(), "enemies") == 0)
        {
            enemies = og;
            break;
        }
    }
    for (auto& e : enemies.children("object"))
    {
        EnemySpawnInfo enemySpawnInfo;
        enemySpawnInfo.position.x = e.attribute("x").as_float() + TILE_SIZE / 2;
        enemySpawnInfo.position.y = e.attribute("y").as_float() + TILE_SIZE / 2;
        enemySpawnInfo.type = e.attribute("type").as_string();
        int room_id = rs.room_id(enemySpawnInfo.position);
        for (auto& p : e.child("properties").children("property"))
        {
            auto name = p.attribute("name").as_string();
            auto value = p.attribute("value");
            if (strcmp(name, "is_oscillating") == 0)
            {
                if (strcmp(value.as_string(), "true") == 0)
                {
                    enemySpawnInfo.is_oscillating = true;
                }
                else
                {
                    enemySpawnInfo.is_oscillating = false;
                }
            }
            if (strcmp(name, "is_circular") == 0)
            {
                if (strcmp(value.as_string(), "true") == 0)
                {
                    enemySpawnInfo.is_circular = true;
                }
                else
                {
                    enemySpawnInfo.is_circular = false;
                }
            }
            if (strcmp(name, "speed") == 0)
            {
                enemySpawnInfo.speed = value.as_int();
                enemySpawnInfo.trace_speed = value.as_float();
            }
            if (strcmp(name, "displacement_x") == 0)
            {
                enemySpawnInfo.displacement.x = value.as_float();
            }
            if (strcmp(name, "displacement_y") == 0)
            {
                enemySpawnInfo.displacement.y = value.as_float();
            }
            if (strcmp(name, "facing_direction") == 0)
            {
                enemySpawnInfo.facing_direction = value.as_string();
            }
			if (strcmp(name, "shoot_spear") == 0)
			{
				if (strcmp(value.as_string(), "true") == 0)
				{
					enemySpawnInfo.shoot_spear = true;
				}
				else
				{
					enemySpawnInfo.shoot_spear = false;
				}
			}
			if (strcmp(name, "trace_speed") == 0)
			{
				enemySpawnInfo.trace_speed = value.as_float();
			}
        }
        //adding enemy information to corresponding rooms
        std::map<int, std::vector<EnemySpawnInfo>>::iterator it = rooms.find(room_id);
        if (it != rooms.end())
        {
            auto enemies = rooms.find(room_id)->second;
            enemies.push_back(enemySpawnInfo);
            it->second = enemies;
        }
        else
        {
            rooms.insert({ room_id,{enemySpawnInfo} });
        }
    }
}

void EnemySpawner::player_respawn_handler(PlayerRespawnEvent ev)
{
    auto enemies_entities = m_enemies_query->get_entities();
    for (auto& e : enemies_entities)
    {
        entity_manager->destroy_entity(e);
    }

    auto bullet_entities = m_bullet_query->get_entities();
    for (auto& e : bullet_entities)
    {
        entity_manager->destroy_entity(e);
    }

    vec2 player_position = { 0.f, 0.f };
    auto player = m_player_query->get_entities();
    for (auto& e : player)
    {
        player_position = entity_manager->get_component_data<PositionComponent>(e).Value;
    }
    int current_room = RoomService::get_instance().room_id(player_position);
    auto it = rooms.find(current_room);
    if (it != rooms.end())
    {
        spawn_enemies_in_room(current_room);
    }
}

void EnemySpawner::update(float elapsed_time)
{
    unordered_set<Entity> player_entities = m_player_query->get_entities();
    for (const Entity& e : player_entities)
    {
        PositionComponent pcomp = entity_manager->get_component_data<PositionComponent>(e);
        current_player_room_id = rs.room_id(pcomp.Value);
    }
    unordered_set<Entity> enemies_entities = m_enemies_query->get_entities();

    // If ths player is not in any room, remove all enemies
    if (current_player_room_id == -1)
    {
        previous_player_room_id = current_player_room_id;
        for (auto& e : enemies_entities)
        {
            entity_manager->destroy_entity(e);
        }
    }
    else
    {
        if (current_player_room_id != previous_player_room_id)
        {
            previous_player_room_id = current_player_room_id;
            for (auto& e : enemies_entities)
            {
                entity_manager->destroy_entity(e);
            }
            std::map<int, std::vector<EnemySpawnInfo>>::iterator it = rooms.find(current_player_room_id);
            if (it != rooms.end())
            {
                spawn_enemies_in_room(current_player_room_id);
            }
        }
    }
}

void EnemySpawner::spawn_enemies_in_room(int room_number)
{
    auto current_room_enemies = rooms.find(room_number)->second;
    for (auto& enemy : current_room_enemies)
    {
        EnemySpawner::make_enemy(entity_manager, enemy);
    }
}

void EnemySpawner::make_enemy(std::shared_ptr<EntityManager> entity_manager, EnemySpawnInfo enemy)
{
    Entity e = entity_manager->create_entity();
    PositionComponent pos(enemy.position);
    entity_manager->add_component_data(e, move(pos));
    if (enemy.facing_direction == "left")
    {
        FacingComponent facing = FacingComponent(FacingDirection::LEFT);
        entity_manager->add_component_data(e, move(facing));
    }
    else
    {
        FacingComponent facing = FacingComponent(FacingDirection::RIGHT);
        entity_manager->add_component_data(e, move(facing));
    }
    AABBComponent aabb({ 0.f, 0.f }, { 16.f, 16.f });
    entity_manager->add_component_data(e, move(aabb));
    RenderableComponent renderable_component = RenderableComponent(enemy.type);
    entity_manager->add_component_data(e, move(renderable_component));
	TileColliderComponent tile_collider = TileColliderComponent();
	entity_manager->add_component_data(e, move(tile_collider));

    // for enemies with oscillation 
    if (enemy.is_oscillating == true)
    {
        OscillatorComponent oc = OscillatorComponent(enemy.position, enemy.displacement, enemy.speed, enemy.is_circular);
        entity_manager->add_component_data(e, move(oc));
		// enemies with is_iscillating == true will not trace enemies
		EnemyComponent enemy_component = EnemyComponent(false, enemy.trace_speed,enemy.shoot_spear);
		entity_manager->add_component_data(e, move(enemy_component));
		VelocityComponent vel({ -0.3, 0.3 }, { 1.0, 1.0 });
		entity_manager->add_component_data(e, move(vel));
    }
	else {
		// enemies with is_iscillating == false will trace enemies
		EnemyComponent enemy_component = EnemyComponent(true, enemy.trace_speed, enemy.shoot_spear);
		entity_manager->add_component_data(e, move(enemy_component));
		GravityComponent gravity_component = GravityComponent();
		entity_manager->add_component_data(e, move(gravity_component));
		VelocityComponent vel({ enemy.trace_speed, 0 }, { 1.0, 1.0 });
		entity_manager->add_component_data(e, move(vel));
	}
}
