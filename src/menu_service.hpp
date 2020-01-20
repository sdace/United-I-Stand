#pragma once

#include "common.hpp"

// Singleton service that stores mouse presses and menu info

class MenuService
{
private:
    static MenuService* instance;

    MenuService()
    {

    }
    vec2 mouse_click_pos;

public:
    static MenuService* getInstance();
    void click(vec2 new_pos);
    vec2 getPos();
};
