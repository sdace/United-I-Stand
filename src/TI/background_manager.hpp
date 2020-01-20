#include "common.hpp"
#include "G-Unit/g_unit.hpp"
#include "G-Unit/util/texture_loader.hpp"
#include <unordered_map>
#define BACKGROUND_PATH PROJECT_SOURCE_DIR "./data/background/"

class BackgroundManager {
public:
    // Singleton instance
    static BackgroundManager& get_instance();

    // Initialize background manager with no backgrounds pre-loaded and scrolling turned on
    bool init(mat3 world_projection);

    // Scroll background relative to velocity x
    void scroll(float x);

    // Render background. If parallax (i.e. more than one layer), layers will scroll at different velocities
    void render();

    // Load background for use at any time. layers > 1 indicates a parallax background with specified number of layers
    // while layers == 1 would be a static background.
    // Name should not have .png extension or any layer number
    // Layers are loaded such that layer 1 is closest to camera
    bool load(std::string name, int layers, int width, int length);

    // Set current background to background with input name
    // Background must have been loaded first
    bool set(std::string);

    // Toggles whether background scrolls or not
    void set_scroll(bool on);

private:
    GLuint parallax_vao;
    GLuint parallax_ebo;
    GLuint parallax_vbo;
    GLuint parallax_tbo;

    bool scrollable;

    // Vector with all the loaded background textures
    vector<GLuint> textures;

    // Used to map background string name to corresponding texture index in the vector
    unordered_map<std::string, std::pair<int, int>> indices;

    // First int refers to the index of the texture of the first layer of a background within the vector,
    // while the second index refers to the number of layers within this background
    std::pair<int, int> current;

    mat3 world_projection;

    // Used for scrolling
    float offset;

    Effect effect;
    Transform transform;
};
