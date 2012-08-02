#ifndef SPRITE_HPP
#define SPRITE_HPP

//
// Class for 2D images, used for the menus and such.
//

#include <string>
#include <boost/shared_ptr.hpp>

#include "resource.hpp"
#include "resourcecache.hpp"
#include "texture.hpp"

struct IDirect3DVertexBuffer9;
struct IDirect3DTexture9;

class Sprite;
typedef boost::shared_ptr<Sprite> SpritePtr;
typedef ResourceCache<Sprite> SpriteCache;

class Sprite : public Resource
{
public:
	// FIXME:  Right now sprites are drawn using static vertex buffers, which 
	// should be OK, since sprites generally do not move.
	// (The exception is of course particles.)
	//
	// FIXME:  Dynamic vertex buffers cause a crash, at least on this computer.
	Sprite();
	~Sprite();

	void render(
		float relX, 
		float relY, 
		float relScaleX = 1.0f, 
		float relScaleY = 1.0f);

	void unload();

private:
	bool loadData(const std::string &filename);

	void updateCoords(
		float x, 
		float y, 
		float width, 
		float height);

	IDirect3DVertexBuffer9 *_vertices;
	//IDirect3DTexture9 *_texture;
	TexturePtr _texture;
};

extern SpriteCache spriteCache;

#endif
