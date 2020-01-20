#pragma once 

#include <common.hpp>
#include "tilemap.hpp"
#include "texture_atlas.hpp"
#include "G-Unit/g_unit.hpp"
#include "tile_importer.hpp"
#include <unordered_set>

class TileRenderManager {
public:
	bool init(mat3 world_projection);
	void animate();
	void render();
	void destroy();
private:
	GLuint tile_vao;
	GLuint tile_ebo;
	GLuint tile_vbo;
	GLuint tile_tbo;

	mat3 world_projection;

	Mesh tile_mesh;
	Effect tile_effect;
	Transform tile_transform;

	TextureAtlas texture_atlas;
	vector<vector<int>> tiles;
};