#pragma once

#include "common.hpp"
#include "TI/tmx_parser/pugixml.hpp"
#include "ECS/entity_manager.hpp"
#include "event_manager.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/checkpoint_component.hpp"
#include "ECS/Components/renderable_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "Events/activate_checkpoint_event.hpp"
#include "Events/player_death_event.hpp"
#include "SoundPlayer/sound_player.hpp"

#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

class CheckpointService
{
public:
    CheckpointService();
    static CheckpointService& get_instance();
    Entity last_checkpoint();
    vec2 get_starting_position();
    void set_starting_position(vec2 position);
    bool set_checkpoint(int id);
    void next_checkpoint();
    void prev_checkpoint();
    void init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager);
private:
    void load_checkpoints();
    void activate_checkpoint_handler(ActivateCheckpointEvent ev);

    vec2 m_starting_position;
    Entity m_last_checkpoint;
    pugi::xml_document doc;
    shared_ptr<EntityManager> m_entity_manager;
    shared_ptr<EventManager> m_event_manager;
    shared_ptr<EntityQuery> m_checkpoint_query;
};