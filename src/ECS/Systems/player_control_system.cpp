#include "ECS/Systems/player_control_system.hpp"
#include "G-Unit/g_unit.hpp"
#include "SoundPlayer/sound_player.hpp"
#include "TI/background_manager.hpp"

using namespace std;

#define BUNNY_JUMP -13.f
#define BUNNY_WEIGHT 1.f
#define SPARTAN_WEIGHT 3.f
#define WHEEL_WEIGHT 1.f
#define GROUND_WEIGHT 4.f
#define MAX_AIR_SPEED 4.f
#define MAX_SPEED 10.f
#define BUNNY_AIR_SPEED 0.75f
#define SPARTAN_AIR_SPEED 0.1f
#define BUNNY_SPEED 1.5f
#define SPARTAN_SPEED 1.f
#define SPEAR_SPEED 7.f
#define WHEEL_FRICTION 0.1f
#define FRICTION 0.25f
#define BOOST_SPEED 6.f
#define WHEEL_BOUNCE_LOSS 1.0f
#define COYOTE_TIME 3

SoundPlayer& pmusic = SoundPlayer::get_instance();
bool spear_sound_played = 0;
bool walking_sound_played = 0;
bool land = 1;


void PlayerControlSystem::init()
{
    EntityQuery query =
        EntityQuery().all(1, ComponentType<PlayerComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);

    pmusic.initialize();
    pmusic.loadsound("jump.wav");
    pmusic.loadsound("throw_spear.wav");
    pmusic.loadsound("wall_hit.wav");
    pmusic.loadsound("footstep_1.wav");
    pmusic.loadsound("light_landing.wav");
    pmusic.loadsound("heavy_landing.wav");
    pmusic.loadsound("boost.wav");
    pmusic.loadsound("slime_landing.wav");
    auto player_respawn_handler = bind(&PlayerControlSystem::player_respawn_handler, this, placeholders::_1);
    event_manager->add_handler<PlayerRespawnEvent>(player_respawn_handler);
}

void PlayerControlSystem::player_respawn_handler(PlayerRespawnEvent ev)
{
    reset_player();
}

void PlayerControlSystem::reset_player()
{
    auto players = m_entity_query->get_entities();

    for (const auto& player : players)
    {
        unboost(player);
        PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(player);
        pcomp.boosted = false;

        if (entity_manager->is_valid_entity(pcomp.Spear))
        {
            entity_manager->destroy_entity(pcomp.Spear);
            pcomp.Spear = Entity::NULL_ENTITY;
        }

        if (entity_manager->has_component<DeathComponent>(player))
        {
            entity_manager->remove_component<DeathComponent>(player);
        }

        entity_manager->set_component_data(player, move(pcomp));

        VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(player);
        velocity.Value = { 0.f, 0.f };
        velocity.Previous = { 0.f, 0.f };
        entity_manager->set_component_data(player, move(velocity));
    }
}

void PlayerControlSystem::update(float elapsed_sec)
{
    auto entities = m_entity_query->get_entities();

    for (const auto& e : entities)
    {
        PlayerInputComponent inputs = entity_manager->get_component_data<PlayerInputComponent>(e);
        PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(e);

        // Unboost
        if (!pcomp.boosted && boosted_state)
        {
            unboost(e);
        }

        // Bounce wheel if on ground
        TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
        if (pcomp.State == Player::WHEEL)
        {
            if (tile_collider.Below || tile_collider.Above)
            {
                VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(e);
                velocity.Value.y = -velocity.Previous.y * WHEEL_BOUNCE_LOSS;
                if (abs(velocity.Value.y) < 0.3f) velocity.Value.y = 0;
                velocity.clamp();
                entity_manager->set_component_data(e, move(velocity));
            }
            if (tile_collider.Left || tile_collider.Right)
            {
                VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(e);
                velocity.Value.x = -velocity.Previous.x * WHEEL_BOUNCE_LOSS;
                if (abs(velocity.Value.x) < 0.3f) velocity.Value.x = 0;
                velocity.clamp();
                entity_manager->set_component_data(e, move(velocity));
            }
        }

        if (tile_collider.Left || tile_collider.Right)
        {
            if (pcomp.boosted)
            {
                pcomp.boosted = false;
                entity_manager->set_component_data(e, move(pcomp));
                unboost(e);
            }
        }

        // Set Coyote Time
        coyote_time = (tile_collider.Below) ? 0 : coyote_time + 1;

        pcomp = entity_manager->get_component_data<PlayerComponent>(e);
        // Character Switching
        if (inputs.one_pressing)
        {
            if (pcomp.State == Player::SPARTAN || pcomp.State == Player::WHEEL)
            {
                switch_to_bunny(e, pcomp);
                event_manager->post(move(CharacterSwapEvent(Player::BUNNY)));
            }
        }
        else if (inputs.two_pressing)
        {
            if (pcomp.State == Player::BUNNY || pcomp.State == Player::WHEEL)
            {
                switch_to_spartan(e, pcomp);
                event_manager->post(move(CharacterSwapEvent(Player::SPARTAN)));
            }
        }
        else if (inputs.three_pressing)
        {
            if (pcomp.State == Player::BUNNY || pcomp.State == SPARTAN)
            {
                switch_to_wheel(e, pcomp);
                event_manager->post(move(CharacterSwapEvent(Player::WHEEL)));
            }
        }
        if (!pcomp.boosted && !transition)
        {
            move_player(e, inputs);
        }

        switch (entity_manager->get_component_data<PlayerComponent>(e).State)
        {
        case Player::BUNNY:
            if (land == 0 && entity_manager->get_component_data<TileColliderComponent>(e).Below == 1)
            {
                pmusic.playsound("light_landing.wav");
            }
            break;
        case Player::SPARTAN:
            if (land == 0 && entity_manager->get_component_data<TileColliderComponent>(e).Below == 1)
            {
                pmusic.soundvolume(-1, 32);
                pmusic.playsound("heavy_landing.wav");
                pmusic.soundvolume(-1, 128);
            }
            break;
        case Player::WHEEL:
            if (land == 0 && entity_manager->get_component_data<TileColliderComponent>(e).Below == 1)
            {
                pmusic.playsound("slime_landing.wav");
            }
            break;
        default:
            break;
        }
        land = entity_manager->get_component_data<TileColliderComponent>(e).Below;

        animateCharacter(e, elapsed_sec);

        // Specific character actions
        pcomp = entity_manager->get_component_data<PlayerComponent>(e);
        if (pcomp.State == Player::BUNNY)
        {
            jump(e, inputs);
        }
        else if (pcomp.State == Player::SPARTAN)
        {
            throw_spear(e, inputs);
        }
        else if (pcomp.State == Player::WHEEL && !pcomp.boosted && !transition)
        {
            boost(e, inputs);
        }

        if (entity_manager->is_valid_entity(pcomp.Spear))
        {
            VelocityComponent vel = entity_manager->get_component_data<VelocityComponent>(pcomp.Spear);
            if (vel.Value.x == 0 && spear_sound_played == 0)
            {
                pmusic.playsound("wall_hit.wav");
                spear_sound_played = 1;
            }
        }
    }
}

void PlayerControlSystem::move_player(Entity e, PlayerInputComponent inputs)
{
    TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);

    VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(e);
    int velocity_direction = (velocity.Value.x > 0) ? 1 : -1;

    if (tile_collider.Below)
    {
        setWeight(e, GROUND_WEIGHT);
        PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(e);
        velocity.Max = { MAX_SPEED, velocity.Max.y };
        float speed = (pcomp.State == Player::BUNNY) ? BUNNY_SPEED : SPARTAN_SPEED;
        velocity.Value.x += speed * (inputs.right_pressed - inputs.left_pressed);


    }
    else
    {
        velocity.Max = { MAX_AIR_SPEED, velocity.Max.y };
        PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(e);
        if (pcomp.State == Player::BUNNY)
        {
            setWeight(e, BUNNY_WEIGHT);
        }
        else
        {
            setWeight(e, (pcomp.State == Player::WHEEL) ? WHEEL_WEIGHT : SPARTAN_WEIGHT);
        }
        velocity.Value.x += ((pcomp.State != Player::SPARTAN) ? BUNNY_AIR_SPEED : SPARTAN_AIR_SPEED) * (inputs.right_pressed - inputs.left_pressed);
    }

    PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(e);
    velocity.Value.x += -velocity.Value.x * ((pcomp.State == Player::WHEEL) ? WHEEL_FRICTION : FRICTION);

    if (abs(velocity.Value.x) > EPSILON)
    {
        FacingComponent facing = entity_manager->get_component_data<FacingComponent>(e);
        int sign = (velocity.Value.x < 0) ? -1 : 1;
        facing.Value = (FacingDirection)sign;
        entity_manager->set_component_data(e, move(facing));
    }
    else
    {
        // Remove negligible velocity
        velocity.Value.x = 0;
    }
    velocity.clamp();
    entity_manager->set_component_data(e, move(velocity));
}

void PlayerControlSystem::jump(Entity e, PlayerInputComponent inputs)
{
    if (inputs.e_pressing || inputs.action_queued)
    {
        KeyboardService* kb = KeyboardService::getInstance();
        kb->unqueue_action();

        TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
        if (tile_collider.Below || coyote_time <= COYOTE_TIME)
        {
            pmusic.playsound("jump.wav");
            VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(e);
            velocity.Value.y = BUNNY_JUMP;

            // With coyote time, we need to account for the fact that gravity is stronger when you're on the ground
            if (coyote_time > 0)
            {
                velocity.Value.y += GRAVITY * GROUND_WEIGHT;
            }

            velocity.Max.x = MAX_AIR_SPEED;
            entity_manager->set_component_data(e, move(velocity));
        }
    }
}

void PlayerControlSystem::boost(Entity e, PlayerInputComponent inputs)
{
    if (inputs.e_pressing || inputs.action_queued)
    {
        KeyboardService* kb = KeyboardService::getInstance();
        kb->unqueue_action();

        TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(e);
        VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(e);
        velocity.Value.x = entity_manager->get_component_data<FacingComponent>(e).Value * BOOST_SPEED;
        velocity.Value.y = 0;
        entity_manager->set_component_data(e, move(velocity));
        PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(e);
        pcomp.boosted = 1;
        boosted_state = 1;
        RenderableComponent boosted_render = RenderableComponent("boosted_wheel");
        setWeight(e, 0.f);

        entity_manager->set_component_data(e, move(pcomp));
        entity_manager->set_component_data(e, move(boosted_render));
        pmusic.playsound("boost.wav");
    }
}

void PlayerControlSystem::unboost(Entity e)
{
    boosted_state = 0;
    PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(e);
    RenderableComponent render = entity_manager->get_component_data<RenderableComponent>(e);
    if (pcomp.State == Player::BUNNY)
    {
        setWeight(e, BUNNY_WEIGHT);
    }
    else if (pcomp.State == Player::SPARTAN)
    {
        setWeight(e, SPARTAN_WEIGHT);
    }
    else if (pcomp.State == Player::WHEEL)
    {
        setWeight(e, WHEEL_WEIGHT);
        render = RenderableComponent("wheel_idle");
    }
    entity_manager->set_component_data(e, move(render));
}

// Create new Spear entity
/* Needs components:
    - Position
    - Facing
    - Velocity
    - AABB
    - Renderable
*/
void PlayerControlSystem::throw_spear(Entity e, PlayerInputComponent inputs)
{
    if (inputs.e_pressing || inputs.action_queued)
    {
        KeyboardService* kb = KeyboardService::getInstance();
        kb->unqueue_action();

        PlayerComponent player = entity_manager->get_component_data<PlayerComponent>(e);

        if (entity_manager->is_valid_entity(player.Spear))
        {
            entity_manager->destroy_entity(player.Spear);
            spear_sound_played = 0;
        }


        pmusic.playsound("throw_spear.wav");


        Entity spear = entity_manager->create_entity();
        PositionComponent pos = entity_manager->get_component_data<PositionComponent>(e);
        pos.Value.y -= 2;
        FacingComponent facing = entity_manager->get_component_data<FacingComponent>(e);
        VelocityComponent vel = VelocityComponent({ facing.Value * SPEAR_SPEED, 0.f }, { facing.Value * SPEAR_SPEED, 0.f });
        AABBComponent aabb = AABBComponent({ 0.f, 0.f }, { 16.f, 5.f });
        SolidComponent solid = SolidComponent(true);
        TileColliderComponent tile_collider{};
        RenderableComponent renderable = RenderableComponent("spear");
		PlayerSpearComponent player_spear = PlayerSpearComponent({16.f,3.f});
        entity_manager->add_component_data(spear, move(facing));
        entity_manager->add_component_data(spear, move(pos));
        entity_manager->add_component_data(spear, move(vel));
        entity_manager->add_component_data(spear, move(aabb));
        entity_manager->add_component_data(spear, move(solid));
        entity_manager->add_component_data(spear, move(tile_collider));
        entity_manager->add_component_data(spear, move(renderable));
        entity_manager->add_component_data(spear, move(player_spear));

        player.Spear = spear;

        entity_manager->set_component_data(e, move(player));

        RenderableComponent new_render = RenderableComponent("spartan_spear_throw");
        entity_manager->set_component_data(e, move(new_render));
    }
}



void PlayerControlSystem::switch_to_bunny(Entity e, PlayerComponent pcomp)
{
    TileColliderComponent tcomp = entity_manager->get_component_data<TileColliderComponent>(e);
    if (!pcomp.boosted && !tcomp.Below)
    {
        setWeight(e, BUNNY_WEIGHT);
    }
    pcomp.State = Player::BUNNY;
    transition = 1;
    RenderableComponent new_render = RenderableComponent("to_bunny");
    entity_manager->set_component_data(e, move(pcomp));
    entity_manager->set_component_data(e, move(new_render));
}

void PlayerControlSystem::switch_to_spartan(Entity e, PlayerComponent pcomp)
{
    setWeight(e, SPARTAN_WEIGHT);
    pcomp.State = Player::SPARTAN;
    transition = 1;
    RenderableComponent new_render = RenderableComponent("to_spartan");
    entity_manager->set_component_data(e, move(pcomp));
    entity_manager->set_component_data(e, move(new_render));
}

void PlayerControlSystem::switch_to_wheel(Entity e, PlayerComponent pcomp)
{
    if (!pcomp.boosted)
    {
        setWeight(e, WHEEL_WEIGHT);
    }
    else
    {
        VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(e);
        velocity.Value.y = 0;
        entity_manager->set_component_data(e, move(velocity));
        setWeight(e, 0.f);
    }
    pcomp.State = Player::WHEEL;
    transition = 1;
    RenderableComponent new_render = RenderableComponent("to_wheel");
    entity_manager->set_component_data(e, move(pcomp));
    entity_manager->set_component_data(e, move(new_render));
}

void PlayerControlSystem::setWeight(Entity e, float weight)
{
    GravityComponent grav = entity_manager->get_component_data<GravityComponent>(e);
    grav.weight = weight;
    entity_manager->set_component_data(e, move(grav));
}

void PlayerControlSystem::animateCharacter(Entity e, float time)
{
    PlayerComponent pcomp = entity_manager->get_component_data<PlayerComponent>(e);
    TileColliderComponent tcomp = entity_manager->get_component_data<TileColliderComponent>(e);
    VelocityComponent vcomp = entity_manager->get_component_data<VelocityComponent>(e);
    RenderableComponent rcomp = entity_manager->get_component_data<RenderableComponent>(e);
    auto& background = BackgroundManager::get_instance();

    bool grounded = tcomp.Below;
    auto state = pcomp.State;
    vec2 velocity = vcomp.Value;
    auto gstate = rcomp.gstate;
    static float elapsed_time = time;
    elapsed_time += time;
    static int frame;

    float VELOCITY_THRESH = 2.0;

    // Transitions between characters
    if (transition)
    {
        frame = (frame) ? frame + 1 : 1;
        if (frame % 3 == 0)
        {
            GUnit::get_instance().advance_frame("to_bunny");
            GUnit::get_instance().advance_frame("to_spartan");
            GUnit::get_instance().advance_frame("to_wheel");
        }
        if (frame >= CHARACTER_SWAP_FRAMES + 1)
        {
            frame = 0;
        }
        if (gstate == "to_bunny")
        {
            if (frame >= CHARACTER_SWAP_FRAMES)
            {
                if (pcomp.boosted)
                {
                    // TODO: Boosted bunny sprite
                    rcomp = RenderableComponent("player_idle");
                }
                else
                {
                    rcomp = RenderableComponent("player_idle");
                }
                entity_manager->set_component_data(e, move(rcomp));
                elapsed_time = 0;
                transition = 0;
            }
        }
        else if (gstate == "to_spartan" || gstate == "spartan_idle_spear" || gstate == "spartan_spear_throw")
        {
            if (frame >= CHARACTER_SWAP_FRAMES)
            {
                rcomp = RenderableComponent("spartan_idle_spear");
                entity_manager->set_component_data(e, move(rcomp));
                elapsed_time = 0;
                transition = 0;
            }
        }
        else if (gstate == "to_wheel")
        {
            if (frame >= CHARACTER_SWAP_FRAMES)
            {
                if (pcomp.boosted)
                {
                    rcomp = RenderableComponent("boosted_wheel");
                }
                else
                {
                    rcomp = RenderableComponent("wheel_idle");
                }
                entity_manager->set_component_data(e, move(rcomp));
                elapsed_time = 0;
                transition = 0;
            }
        }
        return;
    }


    if (abs(velocity.x) < VELOCITY_THRESH && abs(velocity.y) < VELOCITY_THRESH && grounded)
    {
        walking_sound_played = 0;
        switch (state)
        {
        case Player::BUNNY:
            if (gstate == "player_idle") { return; }
            entity_manager->set_component_data(e, move(RenderableComponent("player_idle")));
            break;
        case Player::SPARTAN:
            // TODO: fix. For now, don't reset the sprite after throwing spear. 
            //                        entity_manager->set_component_data(e, move(RenderableComponent("spartan_idle_spear")));
            break;
        case Player::WHEEL:
            entity_manager->set_component_data(e, move(RenderableComponent("wheel_idle")));
            break;
        default:
            return;
        }
    }


    
    if (velocity.x != 0 && grounded)
    {
        switch (state)
        {
        case Player::BUNNY:
            if (gstate == "rabbit_running")
            {
                if (elapsed_time > 200)
                {
                    GUnit::get_instance().advance_frame("rabbit_running");
                    elapsed_time = 0;
                    return;
                }
            }
            entity_manager->set_component_data(e, move(RenderableComponent("rabbit_running")));
            break;
        case Player::SPARTAN:
            if (pcomp.boosted == 0)
            {
                if (gstate == "spartan_running_spear")
                {
                    if (elapsed_time > 200)
                    {
                        GUnit::get_instance().advance_frame("spartan_running_spear");
                        elapsed_time = 0;
                        return;
                    }
                }
                entity_manager->set_component_data(e, move(RenderableComponent("spartan_running_spear")));
            }
            break;
        
        case Player::WHEEL:
            if (abs(velocity.x) < 0.75 * SPARTAN_SPEED)
            {
                entity_manager->set_component_data(e, move(RenderableComponent("wheel_idle")));
                break;
            }
            if (gstate == "wheel_moving")
            {
                if (elapsed_time > 100)
                {
                    GUnit::get_instance().advance_frame("wheel_moving");
                    elapsed_time = 0;
                    return;
                }
            }
            entity_manager->set_component_data(e, move(RenderableComponent("wheel_moving")));
            break;
        default:

            return;
        }
    }
    
    if (velocity.y < 0 && !grounded)
    {
        switch (state)
        {
        case Player::BUNNY:
            entity_manager->set_component_data(e, move(RenderableComponent("rabbit_jumping")));
            break;
        default:
            return;
        }
    }

	if (abs(velocity.x) < VELOCITY_THRESH && abs(velocity.y) < VELOCITY_THRESH && grounded) {
        walking_sound_played = 0;
		switch (state) {
		case Player::BUNNY:
			if (gstate == "player_idle") { return; } 
			entity_manager->set_component_data(e, move(RenderableComponent("player_idle")));
			break;
                case Player::SPARTAN:
// TODO: fix. For now, don't reset the sprite after throwing spear. 
//                        entity_manager->set_component_data(e, move(RenderableComponent("spartan_idle_spear")));
                        break;
                case Player::WHEEL:
                        entity_manager->set_component_data(e, move(RenderableComponent("wheel_idle")));
                        break;
		default:
			return;
		}
	}

	// Scroll background
	if (velocity.x != 0)
	    background.scroll(velocity.x);
    
	if (velocity.y < 0 && !grounded) {
		switch (state) {
		case Player::BUNNY:
			entity_manager->set_component_data(e, move(RenderableComponent("rabbit_jumping")));
			break;
		default:
			return;
		}
	}

	if (velocity.y > 0 && !grounded) {
		switch (state) {
		case Player::BUNNY:
			entity_manager->set_component_data(e, move(RenderableComponent("rabbit_falling")));
			break;
		default:
			return;
		}
	}
    // rotating spartan
    if (velocity.x != 0 && state == Player::SPARTAN && pcomp.boosted == 1)
    {
                if (gstate == "spartan_rotating")
                {
                    if (elapsed_time > 50)
                    {
                        GUnit::get_instance().advance_frame("spartan_rotating");
                        elapsed_time = 0;
                        return;
                    }
                }
                entity_manager->set_component_data(e, move(RenderableComponent("spartan_rotating")));
    }
}
