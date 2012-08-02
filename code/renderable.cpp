#include <d3dx9.h>

#include "renderer.hpp"
#include "renderable.hpp"
#include "entity.hpp"
#include <cmath>

int Renderable::nextId = 0;

Renderable::Renderable()
{
	_renderableId = nextId++;
}

Renderable::~Renderable()
{
}
