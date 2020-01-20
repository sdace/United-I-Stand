#include "background_manager.hpp"

// Singleton instance
BackgroundManager& BackgroundManager::get_instance()
{
    static BackgroundManager instance;
    return instance;
}

// Initialize background manager with no backgrounds pre-loaded and scrolling turned on
bool BackgroundManager::init(mat3 world_projection) {
    this->world_projection = world_projection;
    this->offset = 0;
    this->current = make_pair(0, 0);
    this->scrollable = true;

    uint16_t tile_indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    glGenBuffers(1, &parallax_vbo);
    glGenBuffers(1, &parallax_ebo);
    glGenBuffers(1, &parallax_vao);
    glGenBuffers(1, &parallax_tbo);

    glBindVertexArray(parallax_vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, parallax_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tile_indices), tile_indices, GL_STATIC_DRAW);

    //load shaders
    return effect.load_from_file(shader_path("t2.vs.glsl"), shader_path("t2.fs.glsl"));

}

// Render background. If parallax (i.e. more than one layer), layers will scroll at different velocities
void BackgroundManager::render() {

    int max = current.first + current.second;

    for (int i = current.first; i < max; i++)
    {
        auto& gunit = GUnit::get_instance();
        transform.begin();
        transform.translate({ gunit.camera_position.x ,gunit.camera_position.y });

        vec2 camera_vector = gunit.get_camera_translation();
        vec2 center_vector = gunit.get_camera_centering();

        // Camera
        transform.translate({ camera_vector.x + center_vector.x, camera_vector.y + center_vector.y });
        transform.scale({ SCREEN_WIDTH, SCREEN_HEIGHT });

        glUseProgram(effect.program);
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        if ( !scrollable )
            this->offset = 0;

        std::vector<TexturedVertex> vertices = {
                {{.5f,.5f,0.f}, {1.0f + ((float) i * offset),1.0f }},	//0
                {{.5f,-.5,0.f},  {1.0f + ((float) i * offset),0.0f }},  //1
                {{-.5f,-.5f,0.f}, {0.0f + ((float) i * offset),0.0f }},	//2
                {{-.5f,.5f,0.f}, {0.0f + ((float) i * offset),1.0f }},   //3
        };

        GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
        GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

        glBindVertexArray(parallax_vao);

        glBindBuffer(GL_ARRAY_BUFFER, parallax_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(3 * sizeof(float)));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);
        glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&world_projection); // from world

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    }
}

// Scroll background relative to velocity x
void BackgroundManager::scroll(float x) {
    this->offset += (x * 0.0001f);
}

// Load background for use at any time. layers > 1 indicates a parallax background with specified number of layers
// while layers == 1 would be a static background.
// Name should not have .png extension or any layer number
// Layers are loaded such that layer 1 is closest to camera
bool BackgroundManager::load(std::string name, int layers, int width, int length) {
    int size = this->textures.size();
    for (int i = layers; i > 0 ; i--)
    {
        GLuint texture;
        std::stringstream path_ss;
        path_ss << BACKGROUND_PATH << name << i << ".png";
        const std::string tmp = path_ss.str();
        const char* path = tmp.c_str();
        if (!TextureLoader::load_from_absolute_path(path, texture, width, length))
        {
            printf("BACKGROUND %s FAILED TO LOAD: LAYER %d\n", name.c_str(), i);
            return false;
        }
        this->textures.push_back(texture);
    }
    this->indices[name] = make_pair(size, layers);
    return true;
}

// Set current background to background with input name
// Background must have been loaded first
bool BackgroundManager::set(std::string name) {
    if (indices.find(name) != indices.end())
    {
        this->current = indices[name];
        return true;
    }

    printf("BACKGROUND %s HAS NOT BEEN LOADED\n", name.c_str());
    return false;
}

// Toggles whether background scrolls or not
void BackgroundManager::set_scroll(bool on) {
    this->scrollable = on;
}
