#include <d3d9.h>
#include <d3dx9.h>

#include "sprite.hpp"
#include "graphics.hpp"
#include "vertexformats.hpp"
#include "util.hpp"
#include "common.hpp"

SpriteCache spriteCache;

Sprite::Sprite()
{
	_vertices = 0;
	_texture.reset(new Texture());
}

Sprite::~Sprite()
{
	unload();
}

void Sprite::unload()
{
	util::release(_vertices);
}

bool Sprite::loadData(const std::string &filename)
{
	//if (_dynamic)
	//	usageFlags |= D3DUSAGE_DYNAMIC;

	MYASSERT(graphics->getDevice());

	// Create the vertex buffer.
	HRESULT vbResult = graphics->getDevice()->CreateVertexBuffer(
							4 * sizeof(SpriteVertex), 
							D3DUSAGE_WRITEONLY, 
							SpriteVertex::FVF, 
							D3DPOOL_MANAGED, 
							&_vertices, 
							0);

	if (FAILED(vbResult))
	{
		log("Couldn't load sprite.");
		return false;
	}

	updateCoords(0, 0, 2, 2);

	// Create the texture.  TODO:  Put this in a resource cache.
	/*HRESULT texResult = D3DXCreateTextureFromFile(
							graphics->getDevice(), 
							filename.c_str(), 
							&_texture);*/

	if (!_texture->load(filename))
	{
		log("Couldn't load sprite texture.");
		return false;
	}

	return true;
}

void Sprite::render(
	float relX, 
	float relY, 
	float relScaleX, 
	float relScaleY)
{
	float absX, absY;
	float absScaleX, absScaleY;
	graphics->toAbsoluteCoords(relX, relY, absX, absY);
	graphics->toAbsoluteCoords(relScaleX, relScaleY, absScaleX, absScaleY);

	// FIXME:  This is probably slower than it needs to be.
	updateCoords(absX, absY, absScaleX, absScaleY);
	
	_texture->bind();
	graphics->getDevice()->SetStreamSource(0, _vertices, 0, sizeof(SpriteVertex));
	graphics->getDevice()->SetFVF(SpriteVertex::FVF);
	graphics->getDevice()->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}

void Sprite::updateCoords(
	float x, 
	float y, 
	float width, 
	float height)
{
	const float leftX   = x - 0.5f;
	const float rightX  = x + width - 0.5f;
	const float topY    = y - 0.5f;
	const float bottomY = y + height - 0.5f;
	SpriteVertex *sv;
	_vertices->Lock(0, 0, reinterpret_cast<void **>(&sv), 0);
		sv[0] = SpriteVertex(leftX,  topY,    0.0f, 1.0f, 0.0f, 0.0f);
		sv[1] = SpriteVertex(rightX, topY,    0.0f, 1.0f, 1.0f, 0.0f);
		sv[2] = SpriteVertex(rightX, bottomY, 0.0f, 1.0f, 1.0f, 1.0f);
		sv[3] = SpriteVertex(leftX,  bottomY, 0.0f, 1.0f, 0.0f, 1.0f);
	_vertices->Unlock();
}
