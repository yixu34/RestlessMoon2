#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <string>
#include <boost/shared_ptr.hpp>

#include "audioresource.hpp"
#include "resourcecache.hpp"

class Music;
typedef boost::shared_ptr<Music> MusicPtr;
typedef ResourceCache<Music> MusicCache;

class Music : public AudioResource
{
public:
	Music();
	~Music();

	void play();

private:
	bool loadData(const std::string &filename);
};

extern MusicCache musicCache;

#endif