#include "player_spawn_position_service.hpp"

PlayerSpawnPositionService& PlayerSpawnPositionService::get_instance()
{
    static PlayerSpawnPositionService instance;
    return instance;
}

PlayerSpawnPositionService::PlayerSpawnPositionService()
{
    // empty
}

vec2 PlayerSpawnPositionService::position(int room)
{
    return m_spawn_positions[room];
}

void PlayerSpawnPositionService::init()
{
    using namespace std;
    pugi::xml_parse_result result = doc.load_file(PATH_TMX);
    if (!result)
    {
        cout << "PLAYERSPAWNER::INIT: fail to load the tmx file!" << endl;
        return;
    }

    auto object_groups = doc.child("map").children("objectgroup");
    pugi::xml_node rooms;

    for (auto& og : object_groups)
    {
        auto name = og.attribute("name");
        if (strcmp(name.as_string(), "playerspawns") == 0)
        {
            rooms = og;
            break;
        }
    }
    int id = 0;

    auto& rs = RoomService::get_instance();

    for (auto& r : rooms.children("object"))
    {

        string x_str(r.attribute("x").value());
        stringstream x_ss(x_str);
        float x;
        x_ss >> x;

        string y_str(r.attribute("y").value());
        stringstream y_ss(y_str);
        float y;
        y_ss >> y;

        int room_id = rs.room_id({ x, y });

        m_spawn_positions[room_id] = { x, y };
    }

}