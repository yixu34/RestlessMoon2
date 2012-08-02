#ifndef MESH_HPP
#define MESH_HPP

//
// Wrapper for triangle meshes.  Note that one mesh instance corresponds to
// one mesh file, and so there should be only one instance for a particular
// mesh file residing in the global resource cache.
//

#include <vector>
#include <boost/shared_ptr.hpp>
#include <d3d9.h>
#include <d3dx9.h>

#include "util.hpp"
#include "resource.hpp"
#include "resourcecache.hpp"

class SkinnedMesh;
class Mesh;
typedef boost::shared_ptr<Mesh> MeshPtr;

typedef ResourceCache<Mesh> MeshCache;
extern MeshCache meshCache;

class Mesh : public Resource
{
public:
	Mesh();
	~Mesh();

    void render();
	void renderSubset(int subset);
	void unload();

	inline ID3DXMesh *getData() const { return _meshData; }

	inline void setData(ID3DXMesh *newMesh) { util::release(_meshData);  _meshData = newMesh; }

private:
	bool loadData(const std::string &filename);
    bool optimize(ID3DXBuffer *adjacencyBuffer);

	friend SkinnedMesh;
	ID3DXMesh *_meshData;
};

#endif
