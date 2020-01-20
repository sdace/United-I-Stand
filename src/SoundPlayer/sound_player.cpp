#include "sound_player.hpp"

// TODO
SoundPlayer& SoundPlayer::get_instance()
{
    static SoundPlayer instance;
    return instance;
}


SoundPlayer::SoundPlayer() {

}
void SoundPlayer::initialize()
{


	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
	}



}

void SoundPlayer::loadmusic(std::string const& filename) {

	std::string const path = ""  data_path  "/audio/" + filename;

    music = Mix_LoadMUS(path.c_str());

	if (music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present ",
			audio_path("background_music.wav"));
	}

}

void SoundPlayer::playmusic(int volume)
{
		// Playing background music indefinitely
    Mix_VolumeMusic(volume);
	Mix_PlayMusic(music, -1);
}

void SoundPlayer::loadsound(std::string const& filename)
{
	std::string const path = ""  data_path  "/audio/" + filename;

    audio = Mix_LoadWAV(path.c_str());

	if (audio == nullptr)
	{
		
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present ",
			audio_path("sound.wav"));
	}

	audios.insert(std::make_pair(filename, audio));
	Mix_VolumeChunk(audio,40);

}

void SoundPlayer::playsound(std::string const& filename,int channel, int loop)
{
	if(audios.find(filename) != audios.end())
	{

	Mix_PlayChannel(channel, audios[filename], loop);
	}
}

void SoundPlayer::resumesound(std::string const& filename,int channel)
{
	if(audios.find(filename) != audios.end())
	{
	Mix_Resume(channel);
	}
}

void SoundPlayer::pausesound(std::string const& filename,int channel)
{
	if(audios.find(filename) != audios.end())
	{
	Mix_Pause(channel);
	}
}


void SoundPlayer::soundvolume(int channel, int volume)
{
		// Playing background music indefinitely
    Mix_Volume(channel,volume);
}

int SoundPlayer::time_left(std::string const& filename)
{
    return 0;
}

bool SoundPlayer::is_playing(std::string const& filename) {
    return false;
}

void SoundPlayer::destroy() {

	if (music != nullptr)
		Mix_FreeMusic(music);
	if (audio != nullptr)
		Mix_FreeChunk(audio);

    Mix_CloseAudio();
}