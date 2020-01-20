#pragma once

#include "ECS/Components/renderable_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/player_input_component.hpp"
#include "ECS/Components/gravity_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/tile_collider_component.hpp"
#include "ECS/Components/solid_component.hpp"
#include "ECS/Components/facing_component.hpp"
#include "ECS/Components/hurtbox_component.hpp"
#include "ECS/Components/player_spear_component.hpp"
#include "ECS/Components/death_component.hpp"
#include "ECS/entity_manager.hpp"
#include "Events/character_swap_event.hpp"
#include "Events/player_respawn_event.hpp"
#include "keyboard_service.hpp"



class PlayerControlSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;

    void player_respawn_handler(PlayerRespawnEvent ev);
    void reset_player();

    // Character Switches
    void switch_to_bunny(Entity e, PlayerComponent pcomp);
    void switch_to_spartan(Entity e, PlayerComponent pcomp);
    void switch_to_wheel(Entity e, PlayerComponent pcomp);

    // Actions
    void jump(Entity e, PlayerInputComponent inputs);
    void throw_spear(Entity e, PlayerInputComponent inputs);
    void boost(Entity e, PlayerInputComponent inputs);
    void unboost(Entity e);

    // Helpers
    void setWeight(Entity e, float weight);    
    void move_player(Entity e, PlayerInputComponent inputs);
    void animateCharacter(Entity e, float time);

    // State variables
    bool boosted_state = 0;
    bool transition = 0;
    int coyote_time = 0;
};
