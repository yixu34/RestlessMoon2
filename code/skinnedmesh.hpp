#ifndef SKINNEDMESH_HPP
#define SKINNEDMESH_HPP

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <d3dx9.h>

#include "resource.hpp"
#include "renderable.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "effect.hpp"
#include "allocmeshhierarchy.hpp"

class SkinnedMesh;
typedef boost::shared_ptr<SkinnedMesh> SkinnedMeshPtr;
typedef ResourceCache<SkinnedMesh> SkinnedMeshCache;

extern SkinnedMeshCache skinnedMeshCache;

// This is the resource.
class SkinnedMesh : public Renderable, public Resource
{
public:
	SkinnedMesh();
	~SkinnedMesh();

	void unload();

	void timepass();
	void render();
	void renderSubset(int subset);
	void renderFrames(int subset);

	bool isStatic() const;
	ID3DXMesh *getMeshData() const;
	ID3DXAnimationController *getNewController() const;

private:
	SkinnedMesh(const SkinnedMesh &other);
	SkinnedMesh &operator=(const SkinnedMesh &other);

	bool loadData(const std::string &filename);

	void renderFrame(FRAME *frame, int subset);
	void setupBoneMatrices(FRAME *frame, LPD3DXMATRIX parentMatrix);
	void updateFrameMatrices(FRAME *frame, LPD3DXMATRIX parentMatrix);

	MESHCONTAINER *m_pFirstMesh;			// The first mesh in the hierarchy
	LPD3DXFRAME	   m_pFrameRoot;			// Frame hierarchy of the model
	LPD3DXMATRIX   m_pBoneMatrices;		// Used when calculating the bone position
	UINT           m_uMaxBones;			// The Max number of bones for the model

	//Animation
	LPD3DXANIMATIONCONTROLLER m_pAnimController;// Controller for the animations
};

#endif