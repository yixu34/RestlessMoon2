#ifndef ALLOCMESHHIERARCHY_HPP
#define ALLOCMESHHIERARCHY_HPP

#include <d3dx9.h>

//Derived from the default mesh container
struct MESHCONTAINER: public D3DXMESHCONTAINER
{
	//Mesh variables
	LPDIRECT3DTEXTURE9*  ppTextures;		// Textures of the mesh
	D3DMATERIAL9*		 pMaterials9;		// Use the DirectX 9 Material type

	//Skinned mesh variables
	LPD3DXMESH           pSkinMesh;			// The skin mesh
	LPD3DXMATRIX		 pBoneOffsets;		// The bone matrix Offsets
	LPD3DXMATRIX*		 ppFrameMatrices;	// Pointer to the Frame Matrix

	// Attribute table stuff
	LPD3DXATTRIBUTERANGE pAttributeTable;	// The attribute table
	DWORD                NumAttributeGroups;// The number of attribute groups
};

//Derived frame struct so it looks like a heirarchy
struct FRAME: public D3DXFRAME
{
	D3DXMATRIX matCombined;	//Combined Transformation Matrix

};

class AllocMeshHierarchy : public ID3DXAllocateHierarchy
{
public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_ 
		LPCSTR Name, 
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials, 
		CONST D3DXEFFECTINSTANCE *pEffectInstances, 
		DWORD NumMaterials, 
		CONST DWORD *pAdjacency, 
		LPD3DXSKININFO pSkinInfo, 
		LPD3DXMESHCONTAINER *ppNewMeshContainer);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

	AllocMeshHierarchy() {}
};

#endif