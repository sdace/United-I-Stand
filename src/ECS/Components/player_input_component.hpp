#pragma once

struct PlayerInputComponent
{
    // Possible keyboard presses
    bool up_pressed     = 0;    // arrow keys
    bool down_pressed   = 0;
    bool left_pressed   = 0;
    bool right_pressed  = 0;
    bool one_pressed    = 0;    // switching characters
    bool two_pressed    = 0;
    bool three_pressed  = 0;
    bool e_pressed      = 0;    // action key

    // Current keyboard presses
    bool up_pressing    = 0;    // arrow keys
    bool down_pressing  = 0;
    bool left_pressing  = 0;
    bool right_pressing = 0;
    bool one_pressing   = 0;    // switching characters
    bool two_pressing   = 0;
    bool three_pressing = 0;
    bool e_pressing     = 0;    // action key
    bool action_queued  = 0;
};
