#ifndef AUDIORESOURCE_HPP
#define AUDIORESOURCE_HPP

#include "fmod.hpp"
#include "resource.hpp"

class AudioResource : public Resource
{
public:
	AudioResource();
	virtual ~AudioResource();

	void unload();

	virtual void play() = 0;
	void stop();

	void setLooping(bool looping);
	void setPaused(bool paused);
	void setVolume(float volume);
    float getVolume();
	bool isPlaying() const;

protected:
	FMOD::Sound   *_sound;
	FMOD::Channel *_channel;
};

#endif