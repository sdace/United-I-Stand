#include "menu_service.hpp"

MenuService* MenuService::instance = 0;

vec2 mouse_click_pos;

MenuService* MenuService::getInstance()
{
    if(!instance)
    {
        instance = new MenuService;
    }
    return instance;
}

void MenuService::click(vec2 new_pos)
{
    mouse_click_pos = new_pos;
}

vec2 MenuService::getPos()
{
    return mouse_click_pos;
}
