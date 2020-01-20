#pragma once

#include "TI/room.hpp"

class RoomChangeEvent
{
public:
    RoomChangeEvent(Room room)
    {
        new_room = room;
    }
    Room new_room;
};
