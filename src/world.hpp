#pragma once

// internal
#include "common.hpp"
#include "ECS/entity_manager.hpp"
#include "event_manager.hpp"
#include "keyboard_service.hpp"
#include "TI/tilemap.hpp"
#include "TI/tmx_parser/pugixml.hpp"
#include "spawners/enemy_spawner.hpp"
#include "spawners/platform_spawner.hpp"
#include "spawners/player_spawner.hpp"
#include "spawners/menu_spawner.hpp"
#include "spawners/upgrade_spawner.hpp"
#include "TI/room_service.hpp"
#include "checkpoint_service.hpp"
#include "scene_service.hpp"
#include "menu_service.hpp"
// Components
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/player_input_component.hpp"
#include "ECS/Components/renderable_component.hpp"
#include "ECS/Components/gravity_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/killable_component.hpp"
// Systems
#include "ECS/Systems/checkpoint_system.hpp"
#include "ECS/Systems/movement_system.hpp"
#include "ECS/Systems/player_control_system.hpp"
#include "ECS/Systems/user_input_system.hpp"
#include "ECS/Systems/render_system.hpp"
#include "ECS/Systems/gravity_system.hpp"
#include "ECS/Systems/tile_collision_system.hpp"
#include "ECS/Systems/collision_system.hpp"
#include "ECS/Systems/tile_collision_system.hpp"
#include "ECS/Systems/collision_movement_system.hpp"
#include "ECS/Systems/camera_system.hpp"
#include "ECS/Systems/oscillating_system.hpp"
#include "ECS/Systems/player_death_system.hpp"
#include "ECS/Systems/room_system.hpp"
#include "ECS/Systems/upgrade_system.hpp"
#include "ECS/Systems/enemy_system.hpp"
// G-Unit
#include "G-Unit/g_unit.hpp"

// SoundPlayer
#include "SoundPlayer/sound_player.hpp"

// stlib
#include <vector>
#include <random>
#include <functional>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

using namespace std;

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World
{

public:
    shared_ptr<EntityManager> entity_manager;
    shared_ptr<EventManager> event_manager;

    World();
    ~World();

    // Creates a window, sets up events and begins the game
    bool init(vec2 screen);

    // opengl initialization
    bool render_init(vec2 screen);

    // Start the render loop
    void start_draw();
    // Draw the tiles and finish the render loop
    void end_draw();


    // Releases all associated resources
    void destroy();

    // Should the game be over ?
    bool is_over() const;

    void update(float elapsed_sec);

    template <typename T>
    void register_system(bool can_be_paused)
    {
        auto system = make_shared<T>();
        system->setup(entity_manager, event_manager, can_be_paused);
        m_systems.push_back(system);
        entity_manager->register_system(system);
    }

private:

    void pause_game(int frames);

    void on_key(GLFWwindow*, int key, int, int action, int mod);
    void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
    void character_swap_handler(CharacterSwapEvent ev);
    void upgrade_unlocked_handler(UpgradeUnlockedEvent ev);

private:
    // Window handle
    GLFWwindow* m_window;
    float m_screen_scale; // Screen to pixel coordinates scale factor
    int m_frames; // FPS

    // Screen texture
    // The draw loop first renders to this texture, then it is used for the water shader
    GLuint m_frame_buffer;
    Texture m_screen_tex;
    Effect effect;

    bool paused;
    float pause_timer;

    bool m_debug_mode;

    bool m_spartan_unlocked;
    bool m_wheel_unlocked;

    vector<shared_ptr<EntitySystem>> m_systems;
	EnemySpawner enemy_spawner;
	PlatformSpawner platform_spawner;
	PlayerSpawner player_spawner;
        MenuSpawner menu_spawner;
        UpgradeSpawner upgrade_spawner;

    // Game sounds
};
