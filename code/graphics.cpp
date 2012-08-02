#include <sstream>

#include "graphics.hpp"
#include "common.hpp"
#include "util.hpp"
#include "vertexformats.hpp"
#include "boundingvolume.hpp"
#include "renderer.hpp"
#include "effect.hpp"
#include "text.hpp"

GraphicsPtr graphics;

Graphics::Graphics()
{
	_boundingBoxMesh = 0;
}

Graphics::~Graphics()
{
	util::release(_boundingBoxMesh);
}

bool Graphics::initialize(
	HWND hwnd, 
	D3DDEVTYPE deviceType, 
	int width, 
	int height, 
	bool windowed)
{
	_screenWidth  = width;
	_screenHeight = height;

	IDirect3D9 *d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9 == 0)
	{
		log("Couldn't create the Direct3D9 object.");
		return false;
	}

	// Check for hardware vertex processing.
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

	int vertexProcessing;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	setParameters(hwnd, width, height, windowed);

	if (!createDevice(hwnd, deviceType, d3d9, vertexProcessing))
	{
		log("Couldn't create device.");
		return false;
	}

	// Done with the d3d9 object.
	util::release(d3d9);

	// Set the projection matrix.
	D3DXMATRIX projMatrix;
	D3DXMatrixPerspectiveFovLH(
		&projMatrix, 
		D3DX_PI * 0.5f, 
		static_cast<float>(width) / static_cast<float>(height), 
		1.0f, 
		34000.0f);
	_device->SetTransform(D3DTS_PROJECTION, &projMatrix);

	setTextureParams();

	_currentFont = new CD3DFont(fontDefault.fontName.c_str(), fontDefault.size);
	_currentFont->InitDeviceObjects(_device);
	_currentFont->RestoreDeviceObjects();

	return true;
}

void Graphics::setTextureParams()
{
	graphics->getDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	graphics->getDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	graphics->getDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
}

void Graphics::shutdown()
{
	_currentFont->InvalidateDeviceObjects();
	_currentFont->DeleteDeviceObjects();
	delete _currentFont;
	util::release(_device);
}

void Graphics::setParameters(
	 HWND hwnd, 
	 int width, 
	 int height, 
	 bool windowed)
{
	memset(&_params, 0, sizeof(D3DPRESENT_PARAMETERS));
	_params.BackBufferWidth            = width;
	_params.BackBufferHeight           = height;
	_params.BackBufferFormat           = D3DFMT_A8R8G8B8;
	_params.BackBufferCount            = 1;
	_params.MultiSampleType            = D3DMULTISAMPLE_NONE;
	_params.MultiSampleQuality         = 0;
	_params.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	_params.hDeviceWindow              = hwnd;
	_params.Windowed                   = windowed;
	_params.EnableAutoDepthStencil     = true; 
	_params.AutoDepthStencilFormat     = D3DFMT_D24S8;
	_params.Flags                      = 0;
	_params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	_params.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
}

bool Graphics::createDevice(
							HWND hwnd, 
							D3DDEVTYPE deviceType, 
							IDirect3D9 *d3d9, 
							int vertexProcessing)
{
	HRESULT hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, 
		deviceType, 
		hwnd, 
		vertexProcessing, 
		&_params, 
		&_device);

	return (SUCCEEDED(hr));
}

void Graphics::beginDraw()
{
	//_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	//_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);
	// TODO: Fix skybox model so this doesn't have to happen!
	//_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x000000FF, 1.0f, 0);
	//_device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	_device->BeginScene();
}

void Graphics::endDraw()
{
//	Effect::disableCurrent();

	_device->EndScene();
	_device->Present(0, 0, 0, 0);
}

void Graphics::drawTextAbs(
	float absX, 
	float absY, 
	const std::string &text, 
	bool centered, 
	const TextParams &textParams /* = fontDefault */)
{
	if (centered)
	{
		SIZE fontSize;
		_currentFont->GetTextExtent(text.c_str(), &fontSize);
		float width = fontSize.cx;

		absX -= width / 2;
	}

	_currentFont->DrawText(absX, absY, textParams.color, text.c_str());
}

void Graphics::drawTextRel(
	float relX, 
	float relY, 
	const std::string &text, 
	bool centered, 
	const TextParams &textParams /* = fontDefault */)
{
	float absX, absY;
	toAbsoluteCoords(relX, relY, absX, absY);
	drawTextAbs(absX, absY, text, centered, textParams);
}

void Graphics::calculateTextDimensions(
	const std::string &text, 
	float &absWidth, 
	float &absHeight)
{
	// Extract the font dimensions
	SIZE fontSize;
	_currentFont->GetTextExtent(text.c_str(), &fontSize);
	absWidth  = fontSize.cx;
	absHeight = fontSize.cy;
}

void Graphics::toAbsoluteCoords(
	float relX, 
	float relY, 
	float &absX, 
	float &absY) const
{
	absX = _screenWidth * relX;
	absY = _screenHeight * relY;
}

void Graphics::getMatrices(
	D3DXMATRIX &world, 
	D3DXMATRIX &view, 
	D3DXMATRIX &projection)
{
	_device->GetTransform(D3DTS_WORLD, &world);
	_device->GetTransform(D3DTS_VIEW, &view);
	_device->GetTransform(D3DTS_PROJECTION, &projection);
}

void Graphics::renderBoundingBox(const BoundingBox &box)
{
//	util::release(_boundingBoxMesh);

	static bool initialized = false;
	if (!initialized)
	{
		HRESULT hr = D3DXCreateBox(
			_device, 
			box.max.x - box.min.x, 
			box.max.y - box.min.y,
			box.max.z - box.min.z,
			&_boundingBoxMesh, 
			0);

		MYASSERT(SUCCEEDED(hr));
		initialized = true;
	}

	float boxMidX = (box.max.x + box.min.x) / 2;
	float boxMidY = (box.max.y + box.min.y) / 2;
	float boxMidZ = (box.max.z + box.min.z) / 2;

	D3DXMATRIX worldMatrix;
	D3DXMatrixTranslation(&worldMatrix, boxMidX, boxMidY, boxMidZ);
	_device->SetTransform(D3DTS_WORLD, &worldMatrix);

	HRESULT renderHr = _boundingBoxMesh->DrawSubset(0);
	MYASSERT(SUCCEEDED(renderHr));
}