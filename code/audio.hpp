#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "eventlistener.hpp"
#include <boost/shared_ptr.hpp>
#include <fmod.h>
#include <fmod.hpp>
#include <vector>

//
// Initialization and cleanup code for the audio system. 
// NOTE:  FMOD takes care of releasing all sounds on exit, and so we don't need
// to unload() sound pointers at that time.  However, if you want to manually
// unload() a Sound/Music object, then make sure that happens before the Audio
// class is shutdown().
//

class Audio;
class Sound;
class Music;

typedef boost::shared_ptr<Audio> AudioPtr;
typedef boost::shared_ptr<Sound> SoundPtr;
typedef boost::shared_ptr<Music> MusicPtr;

class Entity;

class Audio: public EventListener
{
public:
	Audio();
	~Audio();

	bool initialize();
	void registerHandlers();
	void shutdown();

	void setMasterVolume(float volume);
	void setMusicVolume(float volume);

	// This must get called once per frame!
	void timepass();

	inline FMOD::System *getDevice() const { return _system; }

private:
	struct SoundToPlay
	{
		SoundPtr sound;
		boost::optional<D3DXVECTOR3> position;
		bool headRelative;
	};

	bool playSound(const Event& soundToPlay);
	bool playMusic(const Event& musicFile);
	void updateListener();
	std::vector<SoundToPlay*> _soundsToPlay;
	MusicPtr _musicFile;
	MusicPtr _interptedMusicFile;
	bool interuptedMusic;
	FMOD::System *_system;
	float _musicVolume;
};

extern AudioPtr audio;

#endif