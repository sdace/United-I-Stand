#pragma once 

#include "common.hpp"
#include "spawners/spawner.hpp"
#include "TI/tmx_parser/pugixml.hpp"
#include "checkpoint_service.hpp"
#include <vector>
#include <iostream>

// every info needed to spawn an enemy
struct PlayerSpawnInfo {
	vec2 position;
	string type;
	string facing_direction;
};

class PlayerSpawner : public Spawner {
public:
	// read the tmx file once, and save all enemy information by room
	void init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager) override;
	void update(float elapsed_time) override;
	void make_player(std::shared_ptr<EntityManager>, PlayerSpawnInfo p);

private:
	pugi::xml_document doc;
};