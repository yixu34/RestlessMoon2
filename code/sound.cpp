#include <d3d9.h>
#include <d3dx9.h>

#include "sound.hpp"
#include "audio.hpp"

SoundCache soundCache;

Sound::Sound()
{
}

Sound::~Sound()
{
	unload();
}

bool Sound::loadData(const std::string &filename)
{
	// NOTE:  Using FMOD_HARDWARE causes sounds to stop playing after a while
	// on my laptop.  What benefits/restrictions come with choosing 
	// FMOD_SOFTWARE or FMOD_HARDWARE?
	FMOD_RESULT result = audio->getDevice()->createSound(
							filename.c_str(), 
							FMOD_HARDWARE | FMOD_3D, 
							0, 
							&_sound);

	return (result == FMOD_OK);
}

void Sound::play3D(const Vector3 &sourcePosition, bool headRelative)
{
	//_channel->setMode(headRelative ? FMOD_3D_HEADRELATIVE : FMOD_3D_WORLDRELATIVE);
	_sound->setMode(headRelative ? FMOD_3D_HEADRELATIVE : FMOD_3D_WORLDRELATIVE);
	audio->getDevice()->playSound(FMOD_CHANNEL_FREE, _sound, true, &_channel);

	Vector3 velocity = {0.0f, 0.0f, 0.0f};
	if (headRelative)
	{
		Vector3 listenerPosition = {0.0f, 0.0f, 0.0f};
		_channel->set3DAttributes(&listenerPosition, &velocity);
	}
	else
		_channel->set3DAttributes(&sourcePosition, &velocity);

	_channel->setPaused(false);
}

void Sound::play()
{
	Vector3 sourcePosition = {0.0f, 0.0f, 0.0f};
	play3D(sourcePosition, true);
}