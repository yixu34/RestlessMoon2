#include <d3d9.h>
#include <d3dx9.h>

#include "dynamictexture.hpp"
#include "graphics.hpp"
#include "util.hpp"

DynamicTextureCache dynamicTextureCache;

DynamicTexture::DynamicTexture()
{
	_texture = 0;
	_depthStencil = 0;
}

DynamicTexture::~DynamicTexture()
{
	unload();
}

bool DynamicTexture::loadData(const std::string &name)
{
	ASSERTMSG(name.length() > 3, "Dynamic Texture cannot be allocated - insufficient arguments in code");

	// First character represents depth/non-depth (texture format)
	bool depthTexture = name[0] == 'd';
	// Second character represents stencil buffer/no stencil buffer
	bool depthStencil = name[1] == 's';
	
	// Find the size
	UINT size = (UINT) std::pow(2,(double)((name[2]-'0')*10 + (name[3]-'0')));
	ASSERTMSG(1 <= size && size <= 2048, "Dynamic Texture must be power of 2 size ranging from 1 to 2048");

	HRESULT hrTexture = graphics->getDevice()->CreateTexture(size, size, 1,
															 D3DUSAGE_RENDERTARGET,
															 depthTexture? D3DFMT_R32F:D3DFMT_A8R8G8B8,
															 D3DPOOL_DEFAULT,
															 &_texture,
															 NULL);
	// If a corresponding depth buffer is requested, give it
	if(depthStencil && SUCCEEDED(hrTexture)) {
		return SUCCEEDED(graphics->getDevice()->CreateDepthStencilSurface(size, size,
																		  D3DFMT_D24S8,
																		  D3DMULTISAMPLE_NONE,
																		  0,
																		  TRUE,
																		  &_depthStencil,
																		  NULL));
	}

	return SUCCEEDED(hrTexture);
}

void DynamicTexture::unload()
{
	util::release(_texture);
	util::release(_depthStencil);
}

bool DynamicTexture::setAsRenderTarget()
{
	// Get the surface of the texture's top mip level (should be the only level)
	//   and set it as the device's active RT.
	IDirect3DSurface9 *renderSurface = NULL;
	if(SUCCEEDED(_texture->GetSurfaceLevel(0,&renderSurface)))
	{
		graphics->getDevice()->SetRenderTarget(0,renderSurface);
		util::release(renderSurface);
	}
	else
		return false;

	// Set the depth stencil if we have one.
	if(_depthStencil)
	{
		graphics->getDevice()->SetDepthStencilSurface(_depthStencil);
	}

	return true;
	
}