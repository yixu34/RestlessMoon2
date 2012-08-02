#include "staticentity.hpp"
#include "renderer.hpp"

StaticEntity::StaticEntity(NewEntityStruct* newEntityParams):Entity(newEntityParams)
{
	_mesh = staticMeshCache.get(newEntityParams->mesh); 
	assert(_mesh);
	renderer->addRenderable(_mesh, _name, this);
}

void StaticEntity::getCameraOffsets(float &offsetX, float &offsetY) const
{
	offsetY = (_boundBox.max.y + _boundBox.min.y) / 2;
	offsetX = (_boundBox.max.x + _boundBox.min.x) / 2;
}

