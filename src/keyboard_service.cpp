#pragma once

#include "keyboard_service.hpp"

KeyboardService* KeyboardService::instance = 0;

unordered_map<int, bool> keys;

KeyboardService* KeyboardService::getInstance()
{
	if(!instance)
	{
		instance = new KeyboardService;
	}
	return instance;
}

bool KeyboardService::isPressed(int input)
{
	return keys[input];
}

void KeyboardService::pressKey(int input)
{
	keys[input] = 1;

    if (paused && (input == e_pressing))
    {
        action_queued = true;
    }
}

void KeyboardService::unpressKey(int input)
{
	keys[input] = 0;
}

void KeyboardService::pause()
{
    paused = true;
}

void KeyboardService::unpause()
{
    paused = false;
}

bool KeyboardService::is_action_queued()
{
    return action_queued;
}

void KeyboardService::unqueue_action()
{
    action_queued = false;
}


