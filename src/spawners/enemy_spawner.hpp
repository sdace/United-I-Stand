#pragma once 

#include "common.hpp"
#include "TI/tmx_parser/pugixml.hpp"
#include "TI/room_service.hpp"
#include "ECS/Components/enemy_spear_component.hpp"
#include "spawners/spawner.hpp"
#include "Events/player_respawn_event.hpp"

#include <map>
#include <vector>
#include <iostream>

// every info needed to spawn an enemy
struct EnemySpawnInfo
{
    vec2 position;
    bool is_oscillating;
    bool is_circular;
    vec2 displacement;
    float speed;
    string facing_direction;
    string type;
	bool shoot_spear;
	float trace_speed;

    // TODO: add more things
    bool dies_to_spear; // etc.
};

class EnemySpawner : public Spawner
{
public:
    // read the tmx file once, and save all enemy information by room
    void init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager) override;
    void update(float elapsed_time) override;

    void spawn_enemies_in_room(int room_number);
    void make_enemy(std::shared_ptr<EntityManager>, EnemySpawnInfo e);

private:
    void player_respawn_handler(PlayerRespawnEvent ev);

    std::map<int, std::vector<EnemySpawnInfo>> rooms; // room id -> list of enemies
    pugi::xml_document doc;
    shared_ptr<EntityManager> entity_manager;
    int previous_player_room_id;
    int current_player_room_id;
    shared_ptr<EntityQuery> m_player_query;
    shared_ptr<EntityQuery> m_enemies_query;
    shared_ptr<EntityQuery> m_bullet_query;
};