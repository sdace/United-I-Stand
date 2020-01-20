// Header
#include "world.hpp"
#include "common.hpp"
#include "TI/tile_importer.hpp"
#include "TI/tilemap.hpp"
#include "TI/tile_render_manager.hpp"
#include "TI/background_manager.hpp"

#include "player_spawn_position_service.hpp"

#include "SoundPlayer/sound_player.hpp"


// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <cstdlib>

using namespace std;
// Same as static in c, local to compilation unit
// fake texture
mat3 world_projection;
shared_ptr<RenderSystem> m_render_system;
TileRenderManager tile_render_manager; // handles tile rendering
namespace
{
    namespace
    {
        void glfw_err_cb(int error, const char* desc)
        {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}

SoundPlayer& music = SoundPlayer::get_instance();
auto& background_manager = BackgroundManager::get_instance();

World::World()
{
    entity_manager = make_shared<EntityManager>();
    event_manager = make_shared<EventManager>();
    m_frame_buffer = 0;
    m_screen_scale = 1;
    m_window = nullptr;
    m_frames = 0; // FPS
    m_debug_mode = false;
    m_spartan_unlocked = false;
    m_wheel_unlocked = false;
}

World::~World()
{


}

// World initialization
bool World::init(vec2 screen)
{
    render_init(screen);
    // INIT TILES -------------------------------------------------------------
    bool tile_init = tile_render_manager.init(world_projection);
    bool back_init = background_manager.init(world_projection);
    background_manager.load("castle", 2, 256, 256);
    background_manager.load("dungeon", 2, 512, 288);
    background_manager.set("dungeon");

    if (!tile_init)
    {
        printf("TILE RENDERING FAILED TO INITIALIZE\n");
        return false;
    }

    if (!back_init)
    {
        printf("BACKGROUND FAILED TO INITIALIZE\n");
        return false;
    }

    // Register systems
    register_system<RoomSystem>(true);
    register_system<PlayerDeathSystem>(true);
    register_system<CheckpointSystem>(true);
    register_system<UpgradeSystem>(true);
    register_system<OscillatingSystem>(true);
    register_system<UserInputSystem>(false);
    register_system<PlayerControlSystem>(false);
    register_system<PreviousMovementSystem>(true);
    register_system<GravitySystem>(true);
    register_system<ResetGroundedSystem>(true);
    register_system<CollisionSystem>(true);
    register_system<CollisionMovementSystem>(true);
    register_system<TileCollisionSystem>(true);
    register_system<MovementSystem>(true);
    register_system<CameraSystem>(false);
    register_system<RenderSystem>(false);
    register_system<EnemySystem>(false);

    // Register event handlers for world
    auto swap_handler = bind(&World::character_swap_handler, this, placeholders::_1);
	event_manager->add_handler<CharacterSwapEvent>(swap_handler);

    auto upgrade_handler = bind(&World::upgrade_unlocked_handler, this, placeholders::_1);
    event_manager->add_handler<UpgradeUnlockedEvent>(upgrade_handler);

    // GUnit initialization: Must happen after ECS initialization
    auto& gunit = GUnit::get_instance();
    gunit.initialize();

	RoomService& rs = RoomService::get_instance();
	rs.init(event_manager);

	// Enemies initialization
	enemy_spawner.init(entity_manager, event_manager);
    // Player initialization
	player_spawner.init(entity_manager, event_manager);
	// Platform initialization

    SceneService* scene_service = SceneService::getInstance();
    MenuService* menu_service = MenuService::getInstance();

    platform_spawner.init(entity_manager, event_manager);
    // Menu initialization
    menu_spawner.init(entity_manager);
    //Upgrade initialization
    upgrade_spawner.init(entity_manager, event_manager);
    // Checkpoint initialization
    CheckpointService::get_instance().init(entity_manager, event_manager);

	// Loading music and sounds
    //SoundPlayer& music = SoundPlayer::get_instance();
    music.initialize();
    music.loadmusic("background_music.wav");
    music.playmusic(50);

    SoundPlayer::get_instance().loadsound("debug_on.wav");
    SoundPlayer::get_instance().loadsound("debug_off.wav");

	fprintf(stderr, "Loaded music\n");



    return true;
}

bool World::render_init(vec2 screen)
{
    // GLFW / OGL Initialization
    // Core Opengl 3.
    glfwSetErrorCallback(glfw_err_cb);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, 0);
    m_window = glfwCreateWindow((int)screen.x * SCREEN_SCALE, (int)screen.y * SCREEN_SCALE, "United I Stand", nullptr, nullptr);
    if (m_window == nullptr)
        return false;

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

    // Load OpenGL function pointers
    gl3w_init();

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(m_window, this);
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_click(wnd, _0, _1, _2); };
    glfwSetKeyCallback(m_window, key_redirect);
    glfwSetMouseButtonCallback(m_window, cursor_pos_redirect);

    int fb_width, fb_height;
    glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
    m_screen_scale = static_cast<float>(fb_width) / screen.x;

    //Texture
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set background color
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)fb_width / m_screen_scale;// *0.5;
    float bottom = (float)fb_height / m_screen_scale;// *0.5;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    mat3 projection{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
    world_projection = projection;
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&world_projection);
    float color2[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color2);

    return true;
}

void World::start_draw() {
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    background_manager.render();
}


void World::end_draw()
{
    if (SceneService::getInstance()->getScene() == Scene::GAME)
    {
        tile_render_manager.render();
    }
    glfwSwapBuffers(m_window);
}

// Releases all the associated resources
void World::destroy()
{
    glDeleteFramebuffers(1, &m_frame_buffer);
	tile_render_manager.destroy();

    
   music.destroy();

	glfwDestroyWindow(m_window);

}

// Should the game be over ?
bool World::is_over() const
{
    return glfwWindowShouldClose(m_window);
}

void World::pause_game(int frames)
{
    paused = true;
    pause_timer = frames;

    KeyboardService* instance = KeyboardService::getInstance();
    instance->pause();
}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    int keyPressed = -1;
    int keyPressing = -1;

    // Ignore keyboard in MENU mode
    if (SceneService::getInstance()->getScene() == MENU)
    {
        return;
    }

    KeyboardService* instance = KeyboardService::getInstance();

    switch (key)
    {
    case GLFW_KEY_UP:
        keyPressed = up_pressed;
        keyPressing = up_pressing;
        break;
    case GLFW_KEY_DOWN:
        keyPressed = down_pressed;
        keyPressing = down_pressing;
        break;
    case GLFW_KEY_LEFT:
        keyPressed = left_pressed;
        keyPressing = left_pressing;
        break;
    case GLFW_KEY_RIGHT:
        keyPressed = right_pressed;
        keyPressing = right_pressing;
        break;
    case GLFW_KEY_1:
        keyPressed = one_pressed;
        keyPressing = one_pressing;
        instance->unpressKey(two_pressing);
        instance->unpressKey(three_pressing);
        break;
    case GLFW_KEY_2:
        if (m_spartan_unlocked || m_debug_mode)
        {
            keyPressed = two_pressed;
            keyPressing = two_pressing;
            instance->unpressKey(one_pressing);
            instance->unpressKey(three_pressing);
        }
        break;
    case GLFW_KEY_3:
        if (m_wheel_unlocked || m_debug_mode)
        {
            keyPressed = three_pressed;
            keyPressing = three_pressing;
            instance->unpressKey(one_pressing);
            instance->unpressKey(two_pressing);
        }
        break;
    case GLFW_KEY_E:
        keyPressed = e_pressed;
        keyPressing = e_pressing;
        break;
    case GLFW_KEY_ESCAPE:
        pause_game(0);
        SceneService::getInstance()->setScene(PAUSED);
        break;
    }

    if (keyPressed != -1)
    {
        if (action == GLFW_PRESS)
        {
            instance->pressKey(keyPressed);
            instance->pressKey(keyPressing);
        }
        else if (action == GLFW_RELEASE)
        {
            instance->unpressKey(keyPressed);
            instance->unpressKey(keyPressing);
        }
    }

    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
    {
        if (m_debug_mode)
        {
            SoundPlayer::get_instance().playsound("debug_off.wav");
        }
        else
        {
            SoundPlayer::get_instance().playsound("debug_on.wav");
        }
        
        m_debug_mode = !m_debug_mode;
    }

    if (m_debug_mode)
    {
        if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
        {
            CheckpointService::get_instance().prev_checkpoint();
        }
        if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
        {
            CheckpointService::get_instance().next_checkpoint();
        }
        if (key == GLFW_KEY_SLASH && action == GLFW_PRESS)
        {
            event_manager->post(PlayerDeathEvent());
        }
    }
}

void World::on_mouse_click(GLFWwindow* window, int button, int action, int mods)
{
    SceneService* instance = SceneService::getInstance();
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xposition, yposition;
        glfwGetCursorPos(window, &xposition, &yposition); 
        vec2 new_pos = {(float)xposition/3, (float)yposition/3};
        // Handle buttons
        switch(instance->getScene())
        {
            case MENU:
                if (inRange(115, 265, new_pos.x) && inRange(85, 125, new_pos.y))
                {
                    instance->setScene(GAME);
                }
                if (inRange(150, 230, new_pos.x) && inRange(130, 150, new_pos.y))
                {
                    instance->setScene(CONTROLS);
                }
                if (inRange(150, 230, new_pos.x) && inRange(155, 175, new_pos.y))
                {
                    instance->setScene(CREDITS);
                }
                if (inRange(150, 230, new_pos.x) && inRange(180, 200, new_pos.y))
                {
                   glfwSetWindowShouldClose(m_window, 1);
                }
                break;
            case PAUSED:
                if (inRange(115, 265, new_pos.x) && inRange(85, 125, new_pos.y))
                {
                    instance->setScene(GAME);
                }
                if (inRange(150, 230, new_pos.x) && inRange(155, 175, new_pos.y))
                {
                    instance->setScene(CONTROLS);
                }
                if (inRange(150, 230, new_pos.x) && inRange(180, 200, new_pos.y))
                {
                   glfwSetWindowShouldClose(m_window, 2);
                }
                break;
            case CONTROLS:
                if (inRange(310, 349, new_pos.x) && inRange(216, 238, new_pos.y))
                {
                    instance->setScene(MENU);
                }
                break;
            case CREDITS:
                // BACK
                if (inRange(310, 349, new_pos.x) && inRange(216, 238, new_pos.y))
                {
                    instance->setScene(MENU);
                }
                break;
            case CONGRATS:
                // RESTART:
                if (inRange(100, 195, new_pos.x) && inRange(140, 195, new_pos.y))
                {
                    instance->setScene(MENU);
                }
            default:
                break;
        }
    }
}

void World::update(float elapsed_time)
{
    // FPS info
    ++m_frames;
    static float time = 0.0f;
    time += elapsed_time;
    if (time > 1000.0f) {
        if (SHOW_FPS_IN_CONSOLE) {
            cout << "FPS: " << m_frames << endl;
        }
        time = 0.0f;
        m_frames = 0;
    }

    // Send projection to GUnit for rendering GStates
    auto& gunit = GUnit::get_instance();
    gunit.set_world_projection(world_projection);

    for (shared_ptr<EntitySystem> system : m_systems)
    {
        if (paused && system->can_be_paused)
        {
            continue;
        }
        system->update(elapsed_time);
    }

	enemy_spawner.update(elapsed_time);

    if (paused && SceneService::getInstance()->getScene() == Scene::GAME)
    {
        pause_timer--;
        if (pause_timer <= 0)
        {
            paused = false;
            pause_timer = 0;
            KeyboardService* instance = KeyboardService::getInstance();
            instance->unpause();
        }
    }
}

void World::character_swap_handler(CharacterSwapEvent ev)
{
    pause_game(CHARACTER_SWAP_FRAMES);
}

void World::upgrade_unlocked_handler(UpgradeUnlockedEvent ev)
{
    SoundPlayer::get_instance().playsound("debug_on.wav");
    if (ev.upgrade_type == UpgradeType::SPARTAN_UPGRADE)
    {
        m_spartan_unlocked = true;
    }
    if (ev.upgrade_type == UpgradeType::WHEEL_UPGRADE)
    {
        m_wheel_unlocked = true;
    }
}
