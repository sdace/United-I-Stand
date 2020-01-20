#pragma once

#include "ECS/entity_manager.hpp"
#include "ECS/Components/renderable_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/gravity_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/player_input_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/solid_component.hpp"
#include "ECS/Components/collider_component.hpp"
#include "ECS/Components/tile_collider_component.hpp"
#include "ECS/Components/facing_component.hpp"
#include "ECS/Components/oscillator_component.hpp"
#include "ECS/Components/killable_component.hpp"
#include "ECS/Components/enemy_component.hpp"
#include "ECS/Components/menu_component.hpp"
#include "ECS/Components/paused_component.hpp"
#include "ECS/Components/controls_component.hpp"
#include "ECS/Components/credits_component.hpp"
#include "ECS/Components/congrats_component.hpp"

#include "G-Unit/g_unit.hpp"

#include "common.hpp"
#include <string>
#include <sstream>

using namespace std;

// Split into more classes if needed
class Spawner
{
public:
    virtual void init(shared_ptr<EntityManager>, shared_ptr<EventManager>) = 0;
    virtual void update(float elapsed_time) = 0;
};

