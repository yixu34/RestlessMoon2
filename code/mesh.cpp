#include "mesh.hpp"
#include "util.hpp"
#include "common.hpp"
#include "graphics.hpp"

MeshCache meshCache;

Mesh::Mesh()
{
    _meshData = 0;
}

Mesh::~Mesh()
{
	unload();
}

void Mesh::unload()
{
	util::release(_meshData);
}

bool Mesh::loadData(const std::string &filename)
{
    MYASSERT(graphics->getDevice());

    ID3DXBuffer *adjacencyBuffer = 0;
    ID3DXBuffer *materialBuffer  = 0;
    DWORD numMaterials           = 0;
    HRESULT loadResult = D3DXLoadMeshFromX(
                             filename.c_str(), 
                             D3DXMESH_MANAGED, 
                             graphics->getDevice(),
                             &adjacencyBuffer, 
                             &materialBuffer, 
                             0, 
                             &numMaterials, 
                             &_meshData);

    if (FAILED(loadResult))
    {
        log("Mesh::load() failed!");
        return false;
    }

	return optimize(adjacencyBuffer);
}

bool Mesh::optimize(ID3DXBuffer *adjacencyBuffer)
{
    DWORD optimizationFlags = D3DXMESHOPT_ATTRSORT |
                              D3DXMESHOPT_COMPACT  |
                              D3DXMESHOPT_VERTEXCACHE;

    HRESULT optimizeResult = _meshData->OptimizeInplace(
                                 optimizationFlags, 
                                 reinterpret_cast<DWORD *>(adjacencyBuffer->GetBufferPointer()), 
                                 0, 
                                 0, 
                                 0);

    if (FAILED(optimizeResult))
    {
        log("OptimizeInplace failed!");
        return false;;
    }
    
    return true;
}

void Mesh::render()
{
    _meshData->DrawSubset(0);
}

void Mesh::renderSubset(int subset)
{
	_meshData->DrawSubset(subset);
}
