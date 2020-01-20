#include "g_unit.hpp"

// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
GUnit& GUnit::get_instance() {
    static GUnit instance;
    return instance;
}

GUnit::GUnit() {
    m_world_projection = { {0,0,0}, {0,0,0},{0,0,0} };
    m_ebo = m_vao = m_vbo = 0;
    camera_position = { 0, 0 };
}

void GUnit::set_camera_position(vec2 pos) {
    RoomService& rs = RoomService::get_instance();
    Room current_room = rs.get_current_room();

    pos.x = max(min(pos.x, current_room.x_max - SCREEN_WIDTH / 2), current_room.x_min + SCREEN_WIDTH / 2);
    pos.y = max(min(pos.y, current_room.y_max - SCREEN_HEIGHT / 2), current_room.y_min + SCREEN_HEIGHT / 2);

    camera_position = pos;
}

vec2 GUnit::get_camera_translation() {
    vec2 camera_transform{ 0, 0 };
    camera_transform.x -= camera_position.x;
    camera_transform.y -= camera_position.y;
    return camera_transform;
}

vec2 GUnit::get_camera_centering() {
    vec2 centering_vector = { 0, 0 };
    centering_vector.x = (float)SCREEN_WIDTH / 2;
    centering_vector.y = (float)SCREEN_HEIGHT / 2;
    return centering_vector;
}

void GUnit::draw_menu(std::string gs) {
    using namespace std;
    auto gstate = get_gstate(gs);
    Transform transform;
    transform.begin();
    transform.translate({ 190, 120 }); // center of screen
    transform.scale({ (float)gstate.image_width, (float)gstate.image_height });


    glUseProgram(gstate.program);

    GLint transform_uloc = glGetUniformLocation(gstate.program, "transform");
    GLint projection_uloc = glGetUniformLocation(gstate.program, "projection");
    
    std::vector<TexturedVertex> vertices = {
        {{.5f,.5f,0.f}, {(float)gstate.current_frame / (float)gstate.frames,1.0f}},	//0
        {{.5f,-.5,0.f},  {(float)gstate.current_frame / (float)gstate.frames,0.0f}},  //1
        {{-.5f,-.5f,0.f}, {((float)gstate.current_frame - 1.0f) / (float)gstate.frames,0.0f}},	//2
        {{-.5f,.5f,0.f}, {((float)gstate.current_frame - 1.0f) / (float)gstate.frames,1.0f}},   //3
    };

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(3 * sizeof(float)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gstate.texture);

    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&m_world_projection); // from world

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void GUnit::draw(std::string gs, Transform transform, PositionComponent position_component, FacingDirection direction)
{
    using namespace std;
    if (!has_gstate(gs)) {
        cout << "GUNIT::DRAW: ERROR: TRIED TO DRAW GSTATE: " << gs << " ... BUT IT DOESN'T EXIST!" << endl;
        return;
    }

    auto gstate = get_gstate(gs);

    transform.begin();
    transform.translate({ position_component.Value.x,position_component.Value.y });

    vec2 camera_vector = get_camera_translation();
    vec2 center_vector = get_camera_centering();

    // Camera
    transform.translate({ camera_vector.x + center_vector.x, camera_vector.y + center_vector.y });
    transform.scale({ direction * (float)gstate.image_width, (float)gstate.image_height });

    glUseProgram(gstate.program);

    std::vector<TexturedVertex> vertices = {
        {{.5f,.5f,0.f}, {(float)gstate.current_frame / (float)gstate.frames,1.0f}},	//0
        {{.5f,-.5,0.f},  {(float)gstate.current_frame / (float)gstate.frames,0.0f}},  //1
        {{-.5f,-.5f,0.f}, {((float)gstate.current_frame - 1.0f) / (float)gstate.frames,0.0f}},	//2
        {{-.5f,.5f,0.f}, {((float)gstate.current_frame - 1.0f) / (float)gstate.frames,1.0f}},   //3
    };

    GLint transform_uloc = glGetUniformLocation(gstate.program, "transform");
    GLint projection_uloc = glGetUniformLocation(gstate.program, "projection");

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(3 * sizeof(float)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gstate.texture);

    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&m_world_projection); // from world

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void GUnit::advance_frame(std::string gstate)
{
    using namespace std;
    if (!has_gstate(gstate)) {
        cout << "GUNIT::ADVANCE_FRAME: ERROR: GState doesn't exist: " << gstate << endl;
        return;
    }

    auto& g = m_gstates[gstate];
    if (g.current_frame == g.frames) {
        g.current_frame = 1;
    }
    else {
        ++g.current_frame;
    }
}

void GUnit::reset_frame(std::string gstate)
{
    using namespace std;
    if (!has_gstate(gstate)) {
        cout << "GUNIT::ADVANCE_FRAME: ERROR: GState doesn't exist: " << gstate << endl;
        return;
    }
    m_gstates[gstate].current_frame = 1;
}

std::vector<std::vector<GStateInfo>> GUnit::get_all_gstate_info() {
    using namespace std;
    namespace fs = filesystem;
    using rdi = fs::recursive_directory_iterator;
    vector<vector<GStateInfo>> gsi;
    for (const auto& file : rdi(gs_path)) {
        gsi.push_back(GStatesFileReader::read_file(file));
    }
    return gsi;
}

void GUnit::load_all_gstates() {
    using namespace std;
    if (VERBOSE) {
        cout << "GUnit::load_all_gstates()" << endl;
    }
    auto gsi = get_all_gstate_info();
    if (VERBOSE) {
        cout << "GUnit::load_all_gstates: get_all_gstate_info returned. # of Files: " << gsi.size() << endl;
    }
    GState s;
    s.current_frame = 1; // initializing it
    for (auto& v : gsi) {
        if (VERBOSE) {
            cout << "GUnit::load_all_gstates: # of states found in current file: " << v.size() << endl;
        }
        for (auto& i : v) {
            if (m_gstates.count(i.name) == 1) {
                cout << "GUNIT::LOAD_ALL_GSTATES MINOR ERROR: ENCOUNTERED TWO GSTATES WITH SAME NAME: " << i.name << endl;
                cout << "(ignoring second one)" << endl;
                continue;
            }
            s.name = i.name;
            s.frames = i.frames;
            if (!get_texture(i.image, s.texture, s.image_width, s.image_height)) {
                cout << "GUNIT::LOAD_ALL_GSTATES: ERROR: FAILED TO LOAD TEXTURE: " << i.image << endl;
            }
            if (!get_program(i.shader, s.program)) {
                cout << "GUNIT::LOAD_ALL_GSTATES: ERROR: FAILED TO LOAD SHADER PROGRAM: " << i.shader << endl;
            }
            s.image_width /= s.frames; // set actual texture width (divide width by frames)
            m_gstates[i.name] = s;
            if (VERBOSE) {
                cout << "GUnit::load_all_gstates: Successfully loaded GState: " << i.name << endl;
            }
        }
    }
}

// Throws if gstate doesn't exist (use has_gstate)
GState& GUnit::get_gstate(const char* gstate) {
    std::string str(gstate);
    return get_gstate(str);
}

// Throws if gstate doesn't exist (use has_gstate)
GState& GUnit::get_gstate(std::string gstate)
{
    using namespace std;
    if (!m_gstates.count(gstate)) {
        cout << "GUNIT::GET_GSTATE: ERROR: The " << gstate << " GState doesn't exist." << endl;
        cout << " (Can use GUnit::has_gstate(" << gstate << ") to check if it exists)" << endl;
        throw "GState doesn't exist. Use has_gstate to check if it exists.";
    }
    return m_gstates[gstate];
}

bool GUnit::has_gstate(std::string state) {
    return m_gstates.count(state);
}

void GUnit::initialize_texture_mesh() {
    std::vector<TexturedVertex> vertices = {
        {{.5f,.5f,0.f}, {1.0f,1.0f}},	//0
        {{.5f,-.5,0.f},  {1.0f,0.0f}},  //1
        {{-.5f,-.5f,0.f}, {0.0f,0.0f}},	//2
        {{-.5f,.5f,0.f}, {0.0f,1.0f}},   //3
    };

    std::vector<Triangle> indices = { {0,1,3}, {1,2,3} };

    // VAO vertex array object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO vertex buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // IBO
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Stride to take text coords into consideration
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Tex coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void GUnit::initialize()
{
    using namespace std;
    if (VERBOSE) {
        cout << endl;
        cout << "GUnit::initialize()" << endl;
    }
    load_all_gstates();
    initialize_texture_mesh();
}

// Do this every frame
void GUnit::set_world_projection(mat3 p) {
    m_world_projection = p;
}

bool gl_compile_shader(GLuint shader)
{
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint log_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        std::vector<char> log(log_len);
        glGetShaderInfoLog(shader, log_len, &log_len, log.data());
        glDeleteShader(shader);

        fprintf(stderr, "GLSL: %s", log.data());
        return false;
    }

    return true;
}

bool GUnit::create_program(std::string shaderName, GLuint& programID) {
    using namespace std;

    string vs_path_str = shader_path_origin + shaderName + ".vs.glsl";
    string fs_path_str = shader_path_origin + shaderName + ".fs.glsl";

    // Opening files
    ifstream vs_is(vs_path_str.c_str());
    ifstream fs_is(fs_path_str.c_str());
    // https://stackoverflow.com/questions/116038/what-is-the-best-way-to-read-an-entire-file-into-a-stdstring-in-c
    string vertex_shader_str(static_cast<stringstream const&>(stringstream() << vs_is.rdbuf()).str());
    string fragment_shader_str(static_cast<stringstream const&>(stringstream() << fs_is.rdbuf()).str());
    if (VERBOSE) {
        cout << "GUnit::load_program: contents of " << shaderName << " vs and fs:" << endl;
        cout << vertex_shader_str << endl;
        cout << fragment_shader_str << endl;
        cout << endl;
    }

    if (!vs_is.good() || !fs_is.good())
    {
        cout << "GUNIT::LOAD_PROGRAM ERROR: A SHADER FILE COULD NOT BE OPENED: " << shaderName << endl;
        return false;
    }

    auto vs_src = vertex_shader_str.c_str();
    auto fs_src = fragment_shader_str.c_str();

    gl_flush_errors();
    auto vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vs_src, nullptr);
    auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fs_src, nullptr);

    // Compiling
    // Shaders already delete if compilation fails
    if (!gl_compile_shader(vertex))
        return false;

    if (!gl_compile_shader(fragment))
    {
        glDeleteShader(vertex);
        return false;
    }

    // Linking
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID); // this sets the program ID
    {
        GLint is_linked = 0;
        glGetProgramiv(programID, GL_LINK_STATUS, &is_linked);
        if (is_linked == GL_FALSE)
        {
            GLint log_len;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &log_len);
            std::vector<char> log(log_len);
            glGetProgramInfoLog(programID, log_len, &log_len, log.data());

            glDeleteProgram(programID);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            fprintf(stderr, "Link error: %s", log.data());
            return false;
        }
    }

    if (gl_has_errors())
    {
        glDeleteProgram(programID);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        fprintf(stderr, "OpenGL errors occured while compiling Effect");
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

bool GUnit::get_program(std::string shaderName, GLuint& programID) {
    using namespace std;
    if (m_programs.count(shaderName) == 0) {
        bool success = create_program(shaderName, programID);
        if (success) {
            if (VERBOSE) {
                cout << "GUnit::load_program: successfully birthed the " << shaderName << " shader program" << endl;
            }
            m_programs[shaderName] = programID;
        }
        else {
            cout << "GUNIT::LOAD_PROGRAM ERROR: SUCCESSFULLY READ SHADER FILES BUT EITHER FAILED COMPILATION OR LINKING: " << shaderName << endl;
        }
        return success;
    }
    else {
        programID = m_programs[shaderName];
        return true;
    }
}

bool GUnit::get_texture(std::string imageName, GLuint& textureID, int& width, int& height) {
    using namespace std;
    if (m_textures.count(imageName) == 0) {
        bool success = TextureLoader::load(imageName, textureID, width, height);
        if (success) {
            m_textures[imageName] = { textureID, width, height };
        }
        else {
            cout << "GUNIT::LOAD_TEXTURE ERROR: SUCCESSFULLY LOADED IMAGE BUT COULD NOT BIND TEXTURE: " << imageName << endl;
        }
        return success;
    }
    else {
        auto triple = m_textures[imageName];
        textureID = get<0>(triple);
        width = get<1>(triple);
        height = get<2>(triple);
        return true;
    }
}

GState::GState()
{
    texture = -1;
    image_width = -1;
    image_height = -1;
    program = -1;
    frames = -1;
    current_frame = -1;
}
