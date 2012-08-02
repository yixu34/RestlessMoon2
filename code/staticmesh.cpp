#include "staticmesh.hpp"
#include "graphics.hpp"
#include "renderer.hpp"
#include "entity.hpp"
#include "common.hpp"
#include "vertexformats.hpp"

StaticMeshCache staticMeshCache;

StaticMesh::StaticMesh()
{
	_mesh.reset(new Mesh());
}

StaticMesh::~StaticMesh()
{
	unload();
}

bool StaticMesh::loadData(const std::string &filename)
{
	_mesh = meshCache.get(filename);
	ASSERTMSG(_mesh, "Mesh load failed.");
	return (_mesh);
}

void StaticMesh::unload() 
{
	_mesh->unload();
}

void StaticMesh::render()
{
//	updatePosition();
	_mesh->render();
}

void StaticMesh::renderSubset(int subset)
{
	_mesh->renderSubset(subset);
}

bool StaticMesh::isStatic() const
{
	return true;
}

ID3DXMesh *StaticMesh::getMeshData() const
{
	return _mesh->getData();
}

void StaticMesh::bakePlanespace()
{
	ID3DXMesh *currMesh = _mesh->getData();
	ID3DXMesh *newMesh = 0;

	HRESULT hr = currMesh->CloneMeshFVF(0,PlanespaceVertex::FVF,
										graphics->getDevice(),&newMesh);

	ASSERTMSG(SUCCEEDED(hr),"Failed to clone mesh to planespace fvf!");

	IDirect3DVertexBuffer9 *meshVB = 0;
	newMesh->GetVertexBuffer(&meshVB);

	PlanespaceVertex *meshData = 0;
	int vertices = newMesh->GetNumVertices();

	meshVB->Lock(0,sizeof(PlanespaceVertex)*vertices,(VOID**)&meshData,D3DLOCK_DISCARD);

	for(int i = 0; i < vertices; i++) {

		PlanespaceVertex *currVertex = meshData + i;

		D3DXVECTOR3 position(currVertex->x,currVertex->y,currVertex->z);
		D3DXVECTOR3 normal(currVertex->nx,currVertex->ny,currVertex->nz);

		float posDotNorm = D3DXVec3Dot(&position,&normal);

		D3DXVECTOR3 v = position - posDotNorm*normal;

		D3DXVECTOR3 seedY(0,1,0);
		if(std::abs(D3DXVec3Dot(&normal,&seedY)) > 0.867) {
			seedY.y = 0;
			seedY.z = 1;
		}

		D3DXVECTOR3 tangentX;
		D3DXVec3Cross(&tangentX,&normal,&seedY);
		D3DXVec3Normalize(&tangentX,&tangentX);

		D3DXVECTOR3 tangentY;
		D3DXVec3Cross(&tangentY,&tangentX,&normal);
	
		currVertex->u = D3DXVec3Dot(&v,&tangentX);
		currVertex->v = D3DXVec3Dot(&v,&tangentY);
	}

	meshVB->Unlock();

	_mesh->setData(newMesh);
}