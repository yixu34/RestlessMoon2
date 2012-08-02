#include "music.hpp"
#include "audio.hpp"

MusicCache musicCache;

Music::Music()
{
}

Music::~Music()
{
}

bool Music::loadData(const std::string &filename)
{
	// See the comment in Sound::loadData() regarding the FMOD_SOFTWARE vs
	// FMOD_HARDWARE issue.
	return (audio->getDevice()->createStream(
		filename.c_str(), 
		FMOD_HARDWARE, 
		0, 
		&_sound) == FMOD_OK);
}

void Music::play()
{
	audio->getDevice()->playSound(FMOD_CHANNEL_FREE, _sound, false, &_channel);
}