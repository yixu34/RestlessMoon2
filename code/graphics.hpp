#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

//
// Controls initialization, shutdown, and main loop of the graphics system.
//

#include <d3d9.h>
#include <d3dx9.h>
#include <boost/shared_ptr.hpp>

#include "d3dfont.h"
#include "text.hpp"

struct BoundingBox;
class Graphics;
typedef boost::shared_ptr<Graphics> GraphicsPtr;

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool initialize(
		HWND hwnd, 
		D3DDEVTYPE deviceType, 
		int width, 
		int height, 
		bool windowed);

	void shutdown();

	void beginDraw();
	void endDraw();

	void drawTextAbs(
		float absX, 
		float absY, 
		const std::string &text,
		bool centered = false,
		const TextParams &textParams = fontDefault);

	void drawTextRel(
		float relX, 
		float relY,
		const std::string &text, 
		bool centered = false,
		const TextParams &textParams = fontDefault);

	void calculateTextDimensions(
		const std::string &text, 
		float &absWidth, 
		float &absHeight);

	void toAbsoluteCoords(
		float relX, 
		float relY, 
		float &absX, 
		float &absY) const;

	void getMatrices(
		D3DXMATRIX &world, 
		D3DXMATRIX &view, 
		D3DXMATRIX &projection);

	void renderBoundingBox(const BoundingBox &box);

	// Not sure if we should keep this or use something in DXUT...
	inline IDirect3DDevice9 *getDevice() const { return _device;       }
	inline float getScreenWidth() const        { return _screenWidth;  }
	inline float getScreenHeight() const       { return _screenHeight; }

private:
	void setParameters(
		HWND hwnd, 
		int width, 
		int height, 
		bool windowed);

	bool createDevice(
		HWND hwnd, 
		D3DDEVTYPE deviceType, 
		IDirect3D9 *d3d9, 
		int vertexProcessing);

	void setTextureParams();

	CD3DFont *_currentFont;

	IDirect3DDevice9 *_device;
	D3DPRESENT_PARAMETERS _params;  // stored in case we change resolutions.

	float _screenWidth;
	float _screenHeight;

	ID3DXMesh *_boundingBoxMesh;
};

extern GraphicsPtr graphics;

#endif
