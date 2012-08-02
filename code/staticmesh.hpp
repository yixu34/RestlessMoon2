#ifndef STATICMESH_HPP
#define STATICMESH_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <d3dx9.h>

#include "mesh.hpp"
#include "renderable.hpp"
#include "resource.hpp"
#include "resourcecache.hpp"

class StaticMesh;
typedef boost::shared_ptr<StaticMesh> StaticMeshPtr;
typedef ResourceCache<StaticMesh> StaticMeshCache;

extern StaticMeshCache staticMeshCache;
class StaticMesh : public Renderable, public Resource
{
public:
	StaticMesh();
	~StaticMesh();

	void unload();
	void render();
	void renderSubset(int subset);
	bool isStatic() const;
	ID3DXMesh *getMeshData() const;

	void bakePlanespace();

private:
	bool loadData(const std::string &filename);

	MeshPtr _mesh;
	/*D3DXVECTOR3 _position;
	float _rotation;*/
};

#endif