#pragma once

#include "ECS/Components/position_component.hpp"
#include "ECS/Components/facing_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/player_input_component.hpp"
#include "ECS/entity_query.hpp"
#include "common.hpp"
#include "G-Unit/util/g_states_file_reader.hpp"
#include "G-Unit/util/texture_loader.hpp"
#include "TI/room_service.hpp"

#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include <tuple>


// Contains everything non-game-logic needed to render something
struct GState {
    GState();
    std::string name;
    GLuint texture;
    int image_width;
    int image_height;
    GLuint program;
    int frames;
    int current_frame; // Not sure if will be used by animation system
};

// Singleton liaison between ECS and OpenGL
// Houses all textures, shader programs
class GUnit {
public:
    // https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
    static GUnit& get_instance();
    GUnit(GUnit const&) = delete;
    void operator= (GUnit const&) = delete;

    GState& get_gstate(const char* gstate);
    GState& get_gstate(std::string gstate);
    bool has_gstate(std::string gstate);

    // Called once per frame per RenderComponent by RenderSystem
    void draw(std::string gstate, Transform transform, PositionComponent position_component, FacingDirection direction);
    // Advance the next frame for animation (automatically loops back to frame 1)
    void advance_frame(std::string gstate);
    // Reset current_frame to 1 (used for starting a one-off animation)
    void reset_frame(std::string gstate);

    // Call once, after GLFW/OpenGL initialization
    void initialize();

    // Call once per frame by world
    void set_world_projection(mat3 p);

    void set_camera_position(vec2 position);

    vec2 get_camera_translation();
    vec2 get_camera_centering();

    vec2 camera_position;

    void draw_menu(std::string gs); // for menu rendering



private:
    // https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
    GUnit();

    void initialize_texture_mesh();
    void load_all_gstates();
    bool get_texture(std::string image, GLuint & textureID, int & width, int & height);
    bool get_program(std::string shader, GLuint& programID);
    bool create_program(std::string program, GLuint& programID);

    std::vector<std::vector<GStateInfo>> get_all_gstate_info();
    std::map<std::string, std::tuple<GLuint, int, int>> m_textures; // triple of texture ID, pixel width, height
    std::map<std::string, GLuint> m_programs; // shader programs
    std::map<std::string, GState> m_gstates;

    // Obtained from world once per frame
    mat3 m_world_projection;
    GLuint m_vao;
    GLuint m_ebo;
    GLuint m_vbo;
};