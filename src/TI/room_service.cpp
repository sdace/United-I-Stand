#include "room_service.hpp"

#include <functional>

using namespace std;

RoomService& RoomService::get_instance() {
    static RoomService instance;
    return instance;
}

RoomService::RoomService() {
}

void RoomService::room_change_handler(RoomChangeEvent ev)
{
    m_current_room = ev.new_room;
}

Room RoomService::get_current_room()
{
    return m_current_room;
}

void RoomService::init_rooms(Room room)
{
    m_current_room = room;
}

// reads all the rooms from the .tmx into the private vector holding the information
void RoomService::init(shared_ptr<EventManager> event_manager) {
    m_event_manager = event_manager;

    auto room_change_handler = bind(&RoomService::room_change_handler, this, placeholders::_1);
    m_event_manager->add_handler<RoomChangeEvent>(room_change_handler);

    using namespace std;
    pugi::xml_parse_result result = doc.load_file(PATH_TMX);
    if (!result) {
        cout << "ROOMSERVICE::INIT: fail to load the tmx file!" << endl;
        return;
    }

    auto object_groups = doc.child("map").children("objectgroup");
    pugi::xml_node rooms;

    for (auto& og : object_groups) {
        auto name = og.attribute("name");
        if (strcmp(name.as_string(), "rooms") == 0) {
            rooms = og;
            break;
        }
    }
    int id = 0;

    for (auto& r : rooms.children("object")) {

        string x_str(r.attribute("x").value());
        stringstream x_ss(x_str);
        float x;
        x_ss >> x;

        string y_str(r.attribute("y").value());
        stringstream y_ss(y_str);
        float y;
        y_ss >> y;

        string w_str(r.attribute("width").value());
        stringstream w_ss(w_str);
        float w;
        w_ss >> w;

        string h_str(r.attribute("height").value());
        stringstream h_ss(h_str);
        float h;
        h_ss >> h;

        Room room;
        room.id = id++;
        room.x_max = x+w;
        room.x_min = x;
        room.y_max = y+h;
        room.y_min = y;
        m_rooms.push_back(room);
    }
}

bool RoomService::is_in_room(vec2 pos, Room & r) {
    return pos.x <= r.x_max && pos.x >= r.x_min && pos.y <= r.y_max && pos.y >= r.y_min;
}

// check all the parsed rooms, whether pos is inside. If not, returns -1.
int RoomService::room_id(vec2 pos) {
    for (auto & r : m_rooms) {
        if (is_in_room(pos, r)) {
            return r.id;
        }
    }
    // not found
    return -1;
}

Room RoomService::get_room(int id)
{
    for (auto& r : m_rooms)
    {
        if (r.id == id)
        {
            return r;
        }
    }
}