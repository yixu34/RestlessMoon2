#ifndef STATICENTITY_HPP
#define STATICENTITY_HPP

#include "entity.hpp"
#include "staticmesh.hpp"
#include <boost/shared_ptr.hpp>

struct BoundingBox;

class StaticEntity;
typedef boost::shared_ptr<StaticEntity> StaticEntityPtr;

class StaticEntity: 
	public Entity
{
	public:
		StaticEntity(NewEntityStruct* newEntityParams);
		void getCameraOffsets(float &offsetX, float &offsetY) const;

	private:
		StaticMeshPtr _mesh;
};

#endif