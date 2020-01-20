#pragma once

#include "../common.hpp"
#include "tmx_parser/pugixml.hpp"
#include "Events/room_change_event.hpp"
#include "event_manager.hpp"
#include "TI/room.hpp"

#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include <sstream>

class RoomService
{
public:
    static RoomService& get_instance();
    RoomService(RoomService const&) = delete;
    void operator= (RoomService const&) = delete;

    void init(shared_ptr<EventManager> event_manager);
    void init_rooms(Room room);
    int room_id(vec2 pos); // position -> room it's in
    Room get_room(int id);
    Room get_current_room();

private:
    RoomService();
    void room_change_handler(RoomChangeEvent ev);
    Room m_current_room;
    Room m_previous_room;

    std::vector<Room> m_rooms;
    pugi::xml_document doc;
    bool is_in_room(vec2 pos, Room& room);

    shared_ptr<EventManager> m_event_manager;
};