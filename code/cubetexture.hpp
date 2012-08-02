#ifndef CUBETEXTURE_HPP
#define CUBETEXTURE_HPP

#include <boost/shared_ptr.hpp>
#include <string>

#include "resource.hpp"
#include "resourcecache.hpp"

struct IDirect3DCubeTexture9;

class Effect;
class CubeTexture;
typedef boost::shared_ptr<CubeTexture> CubeTexturePtr;

class CubeTexture : public Resource
{
public:
	CubeTexture();
	~CubeTexture();

	void unload();

private:
	bool loadData(const std::string &filename);

	friend class Effect;

	IDirect3DCubeTexture9 *_cubeTexture;
};

typedef ResourceCache<CubeTexture> CubeTextureCache;
extern CubeTextureCache cubeTextureCache;

#endif