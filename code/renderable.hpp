#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <boost/shared_ptr.hpp>
#include <vector>
#include <set>

#include "mesh.hpp"
#include "boundingvolume.hpp"

struct D3DXMATRIX;
struct ID3DXMesh;

class Entity;
class Renderable;
typedef boost::shared_ptr<Renderable> RenderablePtr;
typedef std::vector<RenderablePtr> ObjectLayer;

class Renderable
{
public:
	Renderable() ;
	virtual ~Renderable();

	virtual void render() = 0;
	virtual bool isStatic() const = 0;
	virtual ID3DXMesh *getMeshData() const = 0;
	virtual void timepass() {}

	virtual void renderSubset(int subset) { } // optional to override this

	inline int getRenderableId() const { return _renderableId; }

protected:
	//D3DXMATRIX updatePosition();
	void adjustBoundingBox();

	int _renderableId;

	static int nextId;
};


#endif