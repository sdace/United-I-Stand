#pragma once

#include <unordered_map>
#include <iostream>

using namespace std;

// Define key press int values
const int up_pressed = 0;
const int down_pressed = 1;
const int left_pressed = 2;
const int right_pressed = 3;
const int one_pressed = 4;
const int two_pressed = 5;
const int three_pressed = 6;
const int e_pressed = 7;
const int up_pressing = 8;
const int down_pressing = 9;
const int left_pressing = 10;
const int right_pressing = 11;
const int one_pressing = 12;
const int two_pressing = 13;
const int three_pressing = 14;
const int e_pressing = 15;

// Singleton service that stores keypresses during frame

class KeyboardService
{
private:
	static KeyboardService* instance;

	unordered_map<int, bool> keys = 
	{
		{up_pressed, 0},
		{down_pressed, 0},
		{left_pressed, 0},
		{right_pressed, 0},
		{one_pressed, 0},
		{two_pressed, 0},
		{three_pressed, 0},

		{e_pressed, 0},
                {up_pressing, 0},
                {down_pressing, 0},
                {left_pressing, 0},
                {right_pressing, 0},
                {one_pressing, 0},
                {two_pressing, 0},
                {three_pressing, 0},
                {e_pressing, 0}
	};

	KeyboardService()
    {
        paused = false;
        action_queued = false;
    };

    bool paused;
    bool action_queued;

public:
	static KeyboardService* getInstance();

	bool isPressed(int input);
	void pressKey(int input);
	void unpressKey(int input);

    void pause();
    void unpause();

    bool is_action_queued();
    void unqueue_action();
};
