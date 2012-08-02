#include <d3d9.h>
#include <d3dx9.h>

#include "texture.hpp"
#include "graphics.hpp"
#include "util.hpp"

TextureCache textureCache;

Texture::Texture()
{
	_texture = 0;
}

Texture::~Texture()
{
	unload();
}

bool Texture::loadData(const std::string &filename)
{
	return SUCCEEDED(D3DXCreateTextureFromFile(
		graphics->getDevice(),
		filename.c_str(), 
		&_texture));
}

void Texture::unload()
{
	util::release(_texture);
}

void Texture::bind()
{
	graphics->getDevice()->SetTexture(0, _texture);
}