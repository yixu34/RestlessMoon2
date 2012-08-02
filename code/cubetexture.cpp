#include <d3d9.h>
#include <d3dx9.h>

#include "cubetexture.hpp"
#include "graphics.hpp"
#include "util.hpp"

CubeTextureCache cubeTextureCache;

CubeTexture::CubeTexture()
{
	_cubeTexture = 0;
}

CubeTexture::~CubeTexture()
{
	unload();
}

bool CubeTexture::loadData(const std::string &filename)
{
	return SUCCEEDED(D3DXCreateCubeTextureFromFile(
		graphics->getDevice(),
		filename.c_str(), 
		&_cubeTexture));
}

void CubeTexture::unload()
{
	util::release(_cubeTexture);
}