#include "texture_atlas.hpp"
using namespace std;

GLuint TextureAtlas::tile_textures;

TextureAtlas &TextureAtlas::get_instance() {
    static TextureAtlas instance;
    return instance;
}

bool TextureAtlas::init() {
    using namespace std;
    int w, h;
    bool success = TextureLoader::load_from_absolute_path(PATH_IMG, tile_textures, w, h);
    if (success && VERBOSE) {
        cout << "TextureAtlas::init: successfully loaded tiles texture@ " << PATH_IMG << endl;
    }

    Parse();
    debug();
	map_coordinates();

    return true;
}
// Returns texture coordinates within texture atlas of tile with desired index
vec2* TextureAtlas::texCoord(int index) {
	return tex_coords[index];
}

void TextureAtlas::map_coordinates()
{
	for (int index = 0; index < num_tiles; index++)
	{
		// Maps one dimensional tile index to two dimensional matrix indices (texture)
		int m = index % tiles_x;
		int r = (index - m) / tiles_x;

		// Based of previously calculated indices return texture coordinates 0 <= u,v <= 1;
		float u = m * tex_hstride;
		float v = r * tex_vstride;

		// Ordered correctly for texturing
		vec2* coords = new vec2[4];
		coords[2] = { u, v };
		coords[1] = { u + tex_hstride, v };
		coords[0] = { u + tex_hstride, v + tex_vstride };
		coords[3] = { u , v + tex_vstride };

		tex_coords.push_back(coords);
	}
}

// Read information from tileset .tmx file to set values of private variables
void TextureAtlas::Parse() {

    for (const auto& entry : std::filesystem::directory_iterator(PATH_TA)) {

        auto path = entry.path();
        printf("file: %s\n", path.string().c_str());

        if (path.extension() != ".tsx")
            continue;

        ifstream infile;
        infile.open(path);

        string line;

        int image_height = -1;
        int image_width = -1;
        getline(infile, line); // skip the first line that says XML bla bla

        getline(infile, line); // this line contains all the info

            // check for tilewidth attribute
        auto pos = line.find("tilewidth");

        if (pos != string::npos) {
            string str;
            str = line.substr(pos);

            stringstream ss(str);
            // skip the next 11 characters i.e. '   tilewidth="    ' (11 chars)
            for (int i = 0; i < 11; ++i) {
                char c;
                ss >> c;
            }

            ss >> tile_width;
        }
        else {
            cout << "tiles::texture_atlas::Parse:  could not find tilewidth in the file" << endl;
        }

        // check for tileheight attribute
        pos = line.find("tileheight");

        if (pos != string::npos) {
            string str;
            str = line.substr(pos);

            stringstream ss(str); // skip '   tileheight"=   ' (12 chars)
            for (int i = 0; i < 12; ++i) {
                char c;
                ss >> c;
            }

            ss >> tile_height;
        }
        else {
            cout << "tiles::texture_atlas::Parse:  could not find tileheight in the file" << endl;
        }

        // check for tilecount attribute
        pos = line.find("tilecount");

        if (pos != string::npos) {
            string str;
            str = line.substr(pos);

            stringstream ss(str); // skip '   tilecount"=   ' (11 chars)
            for (int i = 0; i < 11; ++i)
            {
                char c;
                ss >> c;
            }

            ss >> num_tiles;
        }
        else {
            cout << "tiles::texture_atlas::Parse:  could not find tilecount in the file" << endl;
        }

        getline(infile, line); // move on to next line

        // check for width attribute
        pos = line.find("width"); // skip '   width"=   ' (7 chars)

        if (pos != string::npos) {
            string str;
            str = line.substr(pos);


            stringstream ss(str);
            for (int i = 0; i < 7; ++i) {
                char c;
                ss >> c;
            }

            ss >> image_width;
        }
        else {
            cout << "tiles::texture_atlas::Parse:  could not find width in the file" << endl;
        }

        // check for height attribute
        pos = line.find("height");

        if (pos != string::npos) {
            string str;
            str = line.substr(pos);

            stringstream ss(str);
            // Skip ' height"= ' ( 8 chars)
            for (int i = 0; i < 8; ++i) {
                char c;
                ss >> c;
            }

            ss >> image_height;
        }
        else {
            cout << "tiles::texture_atlas::Parse:  could not find height in the file" << endl;
        }

        if (image_width == -1 || image_height == -1) {
            cout << "tiles::texture_atlas::Parse:  Failed to parse tsx file" << endl;
            return;
        }

        while (line != "</tileset>")
        {
            getline(infile, line);
            std::string attribute;
            pos = line.find("type");
            if (pos != string::npos)
            {
                string str;
                str = line.substr(pos);

                stringstream ss(str);
                // Skip ' type=" ' ( 6 chars)
                int start = 6;

                auto end_pos = line.find("\"/>");
                auto size = end_pos - pos - 6;

                std::string type = ss.str().substr(start,size);

                pos = line.find("id");

                if (pos != string::npos)
                {
                    str = line.substr(pos);

                    stringstream ss(str);
                    // Skip ' id=" ' ( 4 chars)
                    for (int i = 0; i < 4; ++i) {
                        char c;
                        ss >> c;
                    }

                    int id;
                    ss >> id;

                    attributes[id] = type;
                    continue;
                }

                printf("UNABLE TO FIND MATCHING ID FOR ATTRIBUTE %s IN TSX FILE\n", type.c_str());
                return;
            }
        }

        // calculate values of remaining private variables
        tiles_x = image_width / tile_width;
        tiles_y = image_height / tile_height;

        tex_hstride = 1.f / (float) tiles_x;
        tex_vstride = 1.f / (float) tiles_y;

        // Debug------
        //cout << "tiles::texture_atlas::Parse: Finished parsing the atlas." << endl;

        // -----------
        infile.close();
    }
}

void TextureAtlas::debug() {
    printf("\n ---------------- texture atlas --------------- \n ");
    std::cout << "Imported " << num_tiles << " tiles" << std::endl;
    std::cout << "Tile Width: " << tile_width << std::endl;
    std::cout << "Tile Height: " << tile_height << std::endl;
    std::cout << "Tiles X: " << tiles_x << std::endl;
    std::cout << "Tiles Y: " << tiles_y << std::endl;
    printf("Strides are dx = %f, dy = %f\n", tex_hstride, tex_vstride);
    printf("---------------- finished debugging --------------- \n ");
}

bool TextureAtlas::has_attribute(std::string attribute, int id) {
    std::string type;

    if (attributes.find(id) == attributes.end())
        return false;

    return attribute == attributes[id];
}
