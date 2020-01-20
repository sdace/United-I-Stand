#include "tile_importer.hpp"
#include <vector>
#include <string>
using namespace std;
namespace fs = filesystem;

TileManager& TileManager::get_instance()
{
    static TileManager instance;
    return instance;
}

vector<vector<int>> TileManager::parse_single_tilemap_csv(fs::path path) {
    vector<vector<int>> result;
    ifstream infile;
    infile.open(path);

    string line;
    while (getline(infile, line)) {
        vector<int> row;

        stringstream ss(line);

        // push the ints into the vector, skip the commas
        // essentially row = line.split('_')
        for (int i; ss >> i;) {
            row.push_back(i);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }

        result.push_back(row);
    }
    return result;
}

vector<TileMap> TileManager::load_tilemaps()
{
    vector<TileMap> maps;
    try {
        auto dir = fs::directory_iterator(PATH_TILEMAPS);
        for (const auto& entry : dir) {
            TileMap m;
            auto path = entry.path();
            string filename = path.filename().string();
            m.filename = filename;
            m.tiles = parse_single_tilemap_csv(path);

            // Debugging ----------------------
            /*cout << "tiles::load_tilemaps: finished parsing csv: " << m.filename << endl;
            cout << "  rows: " << m.tiles.size() << endl;
            if (m.tiles.size() == 0) {
                cout << " No rows loaded" << endl;
            }
            else {
                cout << "  cols: " << m.tiles[0].size() << endl;

            }*/
            // ---------------------------------


            m.height = (int)m.tiles.size();

            if (m.height == 0) { // skip this file
                cout << "tiles::load_tilemaps: file skipped (reason: likely not csv): " << m.filename << endl;
                continue;
            }

            m.width = (int)m.tiles[0].size();

            // Skeletal game -- set color map
            m.skeletal_tile[-1] = { 0,0,0 }; // 
            for (int i = 0; i < 100; ++i) {
                m.skeletal_tile[i] = { 0,0,i + 25 / 75.f }; // shades of blue
            }
            // --------------

            maps.push_back(m);
        }
    }
    catch (int err) {
        cout << "tiles::load_tilemaps: Error code: " << err << endl;
        cout << "tiles::load_tilemaps: Folder: " << PATH_TILEMAPS << "does not exist. It needs to be where the .exe file is." << endl;
    }



    return maps;
}

