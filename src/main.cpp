#pragma once

// internal
#include "common.hpp"
#include "world.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

// Global 
World world;
const int width = 600;
const int height = 400;

// Entry point
int main(int argc, char* argv[])
{
    // Initializing world (after renderer.init().. sorry)
    if (!world.init({ SCREEN_WIDTH, SCREEN_HEIGHT }))
    {
        // Time to read the error message
        std::cout << "Press any key to exit" << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }

    auto t = Clock::now();

    // variable timestep loop
    while (!world.is_over())
    {
        // Processes system messages, if this wasn't present the window would become unresponsive
        glfwPollEvents();

        // Calculating elapsed times in milliseconds from the previous iteration
        auto now = Clock::now();
        float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
        t = now;
        world.start_draw();
        world.update(elapsed_sec);
        world.end_draw();
    }

    world.destroy();

    return EXIT_SUCCESS;
}