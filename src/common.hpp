#pragma once

// stlib
#include <fstream> // stdout, stderr..

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>
// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "project_path.hpp"

#define SHOW_FPS_IN_CONSOLE true
#define VERBOSE true // switch to true to see more console msgs


#define PATH_TMX PROJECT_SOURCE_DIR "./assets/tilemap_tmx/a.tmx"

#define shader_path_origin PROJECT_SOURCE_DIR "./shaders/"
#define shader_path(name) PROJECT_SOURCE_DIR "./shaders/" name

#define data_path PROJECT_SOURCE_DIR "./data"
#define gs_path data_path "/characters/"
#define textures_path(name)  data_path "/textures/" name
#define textures_path_origin data_path "/textures/"
#define audio_path(name) data_path  "/audio/" name
#define mesh_path(name) data_path  "/meshes/" name

#define TILE_SIZE 16
#define GRAVITY 0.75f
#define SCREEN_WIDTH 380
#define SCREEN_HEIGHT 240
#define SCREEN_SCALE 3

#define CHARACTER_SWAP_FRAMES 10

#define EPSILON 0.001f

// Not much math is needed and there are already way too many libraries linked (:
// If you want to do some overloads..
struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct color { float r, g, b, a; };
struct mat3 { vec3 c0, c1, c2; };

// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
mat3 mul(const mat3& l, const mat3& r);
vec2 mul(vec2 a, float b);
vec3 mul(mat3 m, vec3 v);
vec2 normalize(vec2 v);
vec2 add(vec2 a, vec2 b);
vec2 sub(vec2 a, vec2 b);
vec2 to_vec2(vec3 v);
float sq_len(vec2 a);
float len(vec2 a);



// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
    vec3 position;
    vec2 texcoord;
};

// Texture wrapper
struct Texture
{
    GLuint id;
    GLuint depth_render_buffer_id;
    int width;
    int height;

    bool is_valid()const; // True if texture is valid
    bool create_from_screen(GLFWwindow const* const window); // Screen texture
};

struct Mesh
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
};

struct Effect
{
    GLuint vertex;
    GLuint fragment;
    GLuint program;

    bool load_from_file(const char* vs_path, const char* fs_path); // load shaders from files and link into program
    void release(); // release shaders and program
};

struct Transform
{
    mat3 out;

    void begin();
    void scale(vec2 scale);
    void rotate(float radians);
    void translate(vec2 offset);
    void end();
};

struct Triangle { uint16_t a, b, c; };
struct Vertex { float x, y, z; };

class Common
{
public:
    static bool are_colliding(float x1, float y1, float halfwidth1, float halfheight1,
                              float x2, float y2, float halfwidth2, float halfheight2);
};

bool inRange(float low, float high, float x); 
