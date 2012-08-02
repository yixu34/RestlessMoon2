#include "audioresource.hpp"
#include "audio.hpp"

AudioResource::AudioResource()
{
	_sound   = 0;
	_channel = 0;
}

AudioResource::~AudioResource()
{
	stop();
	unload();
}

void AudioResource::stop()
{
	if (_channel != 0)
		_channel->stop();
}

void AudioResource::setLooping(bool looping)
{
	if (looping)
	{
		_sound->setMode(FMOD_LOOP_NORMAL);
		_sound->setLoopCount(-1);
		_channel->setLoopCount(-1);
	}
	else
	{
		_sound->setMode(FMOD_LOOP_OFF);
		_sound->setLoopCount(0);
		_channel->setLoopCount(0);
	}
}

void AudioResource::setPaused(bool paused)
{
	_channel->setPaused(paused);
}

void AudioResource::setVolume(float volume)
{
	float oldFrequency;
	float oldVolume;
	float oldPan;
	int   oldPriority;

	_sound->getDefaults(&oldFrequency, &oldVolume, &oldPan, &oldPriority);

	_sound->setDefaults(oldFrequency, volume, oldPan, oldPriority);
	_channel->setVolume(volume);
}

bool AudioResource::isPlaying() const
{
	if (_channel == 0)
		return false;

	bool playing;
	_channel->isPlaying(&playing);
	return playing;
}
float AudioResource::getVolume()
{
    float volume;
    _channel->getVolume(&volume);
    return volume;
}

void AudioResource::unload()
{
	// If the audio system is shutting down, then don't do anything; it will
	// release all sounds automatically.  (Besides, dereferencing sound 
	// pointers would cause a crash.)
	if (!audio || !audio->getDevice())
		return;

	if (_sound != 0)
	{
		_sound->release();
		_sound   = 0;
		_channel = 0;
	}
}