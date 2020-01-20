#pragma once

#include "../common.hpp"
#include "util/sound_loader.hpp"

#include <string>
#include <map>
#include <chrono>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class SoundPlayer {
public:
    static SoundPlayer& get_instance();
    SoundPlayer(SoundPlayer const&) = delete;
    void operator= (SoundPlayer const&) = delete;

    void initialize();

    // filename must have extension ("sound" will be treated as "sound.wav")
    // volume is optional (normal = 50)
    void loadmusic(std::string const& filename);
    void playmusic(int volume = 50);
    void loadsound(std::string const& filename);
    void playsound(std::string const& filename,int channel = -1, int loop = 0);
    void resumesound(std::string const& filename,int channel = -1);
    void pausesound(std::string const& filename,int channel = -1);
    void soundvolume(int channel = -1, int volume = 50);


    bool is_playing(std::string const& filename);

    // how much time (ms) is left for playing current file
    // (if 0 then it's not currently playing)
    int time_left(std::string const& filename);

    void destroy();



private:
    SoundPlayer();
    std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>> m_time_started_playing;
    std::map<std::string, int> m_duration; // ms

  	std::map<std::string, Mix_Chunk*> audios; // vector of pebbles

      // Game sounds
    Mix_Music* music;
    Mix_Chunk* audio;

};