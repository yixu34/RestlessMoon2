#ifndef DYNAMICTEXTURE_HPP
#define DYNAMICTEXTURE_HPP

#include <boost/shared_ptr.hpp>
#include <string>

#include "resource.hpp"
#include "resourcecache.hpp"

struct IDirect3DTexture9;

class Effect;
class DynamicTexture;
typedef boost::shared_ptr<DynamicTexture> DynamicTexturePtr;

class DynamicTexture : public Resource
{
public:
	DynamicTexture();
	~DynamicTexture();

	void unload();
	bool setAsRenderTarget();

private:
	bool loadData(const std::string &name);

	friend class Effect;

	IDirect3DTexture9 *_texture;
	IDirect3DSurface9 *_depthStencil;
};

typedef ResourceCache<DynamicTexture> DynamicTextureCache;
extern DynamicTextureCache dynamicTextureCache;

#endif