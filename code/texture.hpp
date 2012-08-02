#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <boost/shared_ptr.hpp>
#include <string>

#include "resource.hpp"
#include "resourcecache.hpp"

struct IDirect3DTexture9;

class Effect;
class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;

class Texture : public Resource
{
public:
	Texture();
	~Texture();

	void unload();
	void bind();

private:
	bool loadData(const std::string &filename);

	friend class Effect;

	IDirect3DTexture9 *_texture;
};

typedef ResourceCache<Texture> TextureCache;
extern TextureCache textureCache;

#endif