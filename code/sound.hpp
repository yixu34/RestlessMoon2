#ifndef SOUND_HPP
#define SOUND_HPP

#include <string>
#include <boost/shared_ptr.hpp>

#include "audioresource.hpp"
#include "resourcecache.hpp"

typedef FMOD_VECTOR Vector3;

class Sound;
typedef boost::shared_ptr<Sound> SoundPtr;
typedef ResourceCache<Sound> SoundCache;

class Sound : public AudioResource
{
public:
	Sound();
	~Sound();

	void play();
	void play3D(const Vector3 &sourcePosition, bool headRelative);

private:
	bool loadData(const std::string &filename);

	static const Vector3 defaultVelocity;
};

extern SoundCache soundCache;

#endif