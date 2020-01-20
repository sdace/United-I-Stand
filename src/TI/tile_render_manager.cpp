#include "tile_render_manager.hpp"

bool TileRenderManager::init(mat3 world_projection) {
	this->world_projection = world_projection;

	texture_atlas = TextureAtlas::get_instance();
	texture_atlas.init();
    TextureAtlas::get_instance().init();

	vector<TileMap> tilemaps = TileManager::get_instance().load_tilemaps();
	TileManager::get_instance().tile_map = tilemaps[0];

	this->tiles = TileManager::get_instance().tile_map.tiles;

	Vertex tile_vertices[4];
	tile_vertices[0] = { 1, 1, 0 };
	tile_vertices[1] = { 1, -1, 0 };
	tile_vertices[2] = { -1, -1, 0 };
	tile_vertices[3] = { -1, 1, 0 };

	uint16_t tile_indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenBuffers(1, &tile_vbo);
	glGenBuffers(1, &tile_ebo);
	glGenBuffers(1, &tile_vao);
	glGenBuffers(1, &tile_tbo);

	glBindVertexArray(tile_vao);

	glBindBuffer(GL_ARRAY_BUFFER, tile_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, tile_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tile_indices), tile_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//load shaders
	if (!tile_effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	return true;
}

void TileRenderManager::render() {
	glUseProgram(tile_effect.program);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
    auto& gunit = GUnit::get_instance();
	tile_transform.begin();
	vec2 camera_vector = gunit.get_camera_translation();
	vec2 center_vector = gunit.get_camera_centering();

	// Camera
	tile_transform.translate({ camera_vector.x + center_vector.x, camera_vector.y + center_vector.y });
	tile_transform.scale({ 8,8 });

	GLint projection_uloc = glGetUniformLocation(tile_effect.program, "projection");
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)& world_projection);

	// Enabling texture and binding it to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_atlas.tile_textures);

	float sh = (SCREEN_HEIGHT / 2) + 16;
	float sw = (SCREEN_WIDTH / 2) + 16;

	// calculate range of indices within current view to eliminate checks within loop
	int min_i = -1 * floor((sh - gunit.camera_position.y) / TILE_SIZE);
	int min_j = -1 * floor((sw - gunit.camera_position.x) / TILE_SIZE);
	int max_i = floor((sh + gunit.camera_position.y) / TILE_SIZE);
	int max_j = floor((sw + gunit.camera_position.x) / TILE_SIZE);

	for (int i = min_i; i < max_i; i++) {
		for (int j = min_j; j < max_j; j++) {

			int tile = tiles[i][j];
			if (tile == -1) {
				continue;
			}

			// translate by tile index
			mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ ((float)j) * 16 + 8, ((float)i) * 16 + 8, 1.f } };
			T = mul(T,tile_transform.out);

			vec2* tex_coords = texture_atlas.texCoord(tile);

			GLint transform_uloc  = glGetUniformLocation(tile_effect.program, "transform");

			glBindVertexArray(tile_vao);

			glBindBuffer(GL_ARRAY_BUFFER, tile_vbo);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			// Create and bind texture buffer
			glBindBuffer(GL_ARRAY_BUFFER, tile_tbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 4, tex_coords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

			glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)& T);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}
	}
}

void TileRenderManager::destroy() {
	glDeleteBuffers(1, &tile_mesh.vbo);
	glDeleteBuffers(1, &tile_mesh.ibo);
	glDeleteBuffers(1, &tile_mesh.vao);
}