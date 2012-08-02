#ifndef RESOURCE_HPP
#define RESOURCE_HPP

//
// Base class for media objects.  (Meshes, textures, skins, sounds, etc.)
//

#include <boost/shared_ptr.hpp>
#include <string>

class Resource;
typedef boost::shared_ptr<Resource> ResourcePtr;

class Resource
{
public:
	Resource() {};

	// WARNING:  Do NOT try to call unload() in here!  This is because 
	// unload() is a (pure) virtual function, and by the time that the
	// base class destructor is called, the derived class part of the
	// object is invalid!
	virtual ~Resource() {};  

	bool load(const std::string &filename)
	{
		_lastFilename = filename;
		unload();
		return loadData(filename);
	}

	bool reload()
	{
		return load(_lastFilename);
	}

	virtual void unload() = 0;

protected:
	virtual bool loadData(const std::string &filename) = 0;

	std::string _lastFilename;
};

#endif