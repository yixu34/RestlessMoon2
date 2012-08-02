#ifndef TEXT_HPP
#define TEXT_HPP

#include <d3d9.h>
#include <d3dx9.h>
#include "d3dfont.h"
#include <string>

struct TextParams
{
	TextParams(
		const std::string &fontName, 
		int size, 
		const D3DXCOLOR &color)
	{
		this->fontName = fontName;
		this->size     = size;
		this->color    = color;
	}

	std::string fontName;
	int size;
	D3DXCOLOR color;
};

extern TextParams fontDefault;
extern TextParams fontWhite;

#endif
