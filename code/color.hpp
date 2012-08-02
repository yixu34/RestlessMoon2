#ifndef COLOR_HPP
#define COLOR_HPP

//
// Some global constants for colors.
//

#include <d3d9.h>
#include <d3dx9.h>

namespace color
{
	//
	// Some colors in RGBA order.
	//                       R     G     B     A
	const D3DXCOLOR WHITE(  1.0f, 1.0f, 1.0f, 1.0f);
	const D3DXCOLOR BLACK(  0.0f, 0.0f, 0.0f, 1.0f);

	const D3DXCOLOR RED(    1.0f, 0.0f, 0.0f, 1.0f);
	const D3DXCOLOR GREEN(  0.0f, 1.0f, 0.0f, 1.0f);
	const D3DXCOLOR BLUE(   0.0f, 0.0f, 1.0f, 1.0f);

	const D3DXCOLOR YELLOW( 1.0f, 1.0f, 0.0f, 1.0f);
	const D3DXCOLOR MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);
	const D3DXCOLOR CYAN(   0.0f, 1.0f, 1.0f, 1.0f);
	

	//
	// The same colors, but in XRGB order.  (To satisfy the FVF requirement.)
	//                                       R    G    B
	const D3DXCOLOR X_WHITE(  D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR X_BLACK(  D3DCOLOR_XRGB(  0,   0,   0));

	const D3DXCOLOR X_RED(    D3DCOLOR_XRGB(255,   0,   0));
	const D3DXCOLOR X_GREEN(  D3DCOLOR_XRGB(  0, 255,   0));
	const D3DXCOLOR X_BLUE(   D3DCOLOR_XRGB(  0,   0, 255));

	const D3DXCOLOR X_YELLOW( D3DCOLOR_XRGB(255, 255,   0));
	const D3DXCOLOR X_MAGENTA(D3DCOLOR_XRGB(255,   0, 255));
	const D3DXCOLOR X_CYAN(   D3DCOLOR_XRGB(  0, 255, 255));
};

#endif
