#include "allocmeshhierarchy.hpp"
#include "skinnedmesh.hpp"
#include "util.hpp"

HRESULT copyString(const char* input, char** output)
{
	if( input )
	{
		UINT length = (UINT)::strlen(input) + 1; // add 1 for terminating null charater.
		*output = new char[length];
		::strcpy(*output, input);
	}
	else
	{
		*output = 0;
	}
	return S_OK;
}

/////////////////////////////////////////////////
//
// File: "AllocMeshHierarchy.cpp"
//
// Author: Jason Jurecka
//
// Creation Date: June 9, 2003
//
// Purpose: This is an Allocation class that is
//		used with the D3DXLoadMeshHierarchyFromX
//		function. It handles the Creation and Deletion
//		of Frames and Mesh Containers. The overloaded
//		functions are callbacks, so there is no need
//		to call any of the functions in written code
//		just pass an Object of this class to the function
/////////////////////////////////////////////////
#include "allocmeshhierarchy.hpp"

HRESULT AllocMeshHierarchy::CreateFrame(LPCTSTR Name, LPD3DXFRAME *ppNewFrame)
{    
	// Create a frame
	// Using my drived struct here
	FRAME * pFrame = new FRAME;
	ZeroMemory(pFrame, sizeof(FRAME));

	// Inicilize the passed in frame
	*ppNewFrame = NULL;

	// Put the name in the frame
	if(Name)
	{
		int nNameSize = strlen(Name)+1;
		pFrame->Name = new char[nNameSize];
		memcpy(pFrame->Name, Name, nNameSize*sizeof(char));
	}
	else
		pFrame->Name = NULL;

	// Inicilize the rest of the frame
	pFrame->pFrameFirstChild = NULL;
	pFrame->pFrameSibling = NULL;
	pFrame->pMeshContainer = NULL;
	D3DXMatrixIdentity(&pFrame->matCombined);
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	// Set the output frame to the one that we have
	*ppNewFrame = (LPD3DXFRAME)pFrame;

	// It no longer points to the frame
	pFrame = NULL;

	// Returns an HRESULT so give it the AOk result
	return S_OK;
}

HRESULT AllocMeshHierarchy::CreateMeshContainer(
	LPCSTR Name, 
	CONST D3DXMESHDATA *pMeshData,
	CONST D3DXMATERIAL *pMaterials, 
	CONST D3DXEFFECTINSTANCE *pEffectInstances, 
	DWORD NumMaterials, 
	CONST DWORD *pAdjacency, 
	LPD3DXSKININFO pSkinInfo, 
	LPD3DXMESHCONTAINER *ppNewMeshContainer) 
{

	// Create a Temp mesh contianer
	// Using my drived struct here
	MESHCONTAINER * pMeshContainer = new MESHCONTAINER;
	ZeroMemory(pMeshContainer, sizeof(MESHCONTAINER));

	// Inicialize passed in Container
	*ppNewMeshContainer = NULL;

	if(Name)
	{
		// Put in the name
		int nNameSize = strlen(Name)+1;
		pMeshContainer->Name = new char[nNameSize];
		memcpy(pMeshContainer->Name, Name, nNameSize*sizeof(char));
	}
	else
		pMeshContainer->Name = NULL;

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	// Get the number of Faces for adjacency
	DWORD dwFaces = pMeshData->pMesh->GetNumFaces();

	//Get Initcilize all the other data
	pMeshContainer->NumMaterials = NumMaterials;

	//Create the arrays for the materials and the textures
	pMeshContainer->pMaterials9 = new D3DMATERIAL9[pMeshContainer->NumMaterials];

	// Multiply by 3 because there are three adjacent triangles
	pMeshContainer->pAdjacency = new DWORD[dwFaces*3];
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * dwFaces*3);


	//Get the device to use
	LPDIRECT3DDEVICE9 pd3dDevice = NULL;// Direct3D Rendering device
	pMeshData->pMesh->GetDevice(&pd3dDevice);

	pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED, 
		pMeshData->pMesh->GetFVF(), pd3dDevice, 
		&pMeshContainer->MeshData.pMesh);

	pMeshContainer->ppTextures  = new LPDIRECT3DTEXTURE9[NumMaterials];
	for(DWORD dw = 0; dw < NumMaterials; ++dw)
	{
		pMeshContainer->ppTextures [dw] = NULL;

		if(pMaterials[dw].pTextureFilename && strlen(pMaterials[dw].pTextureFilename) > 0)
		{
			if(FAILED(D3DXCreateTextureFromFile(pd3dDevice, 
				pMaterials[dw].pTextureFilename, &pMeshContainer->ppTextures[dw])))
				pMeshContainer->ppTextures [dw] = NULL;
		}
	}

	//Release the device
	util::release(pd3dDevice);

	if(pSkinInfo)
	{
		// first save off the SkinInfo and original mesh data
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// Will need an array of offset matrices to move the vertices from 
		//	the figure space to the bone's space
		UINT uBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsets = new D3DXMATRIX[uBones];

		//Create the arrays for the bones and the frame matrices
		pMeshContainer->ppFrameMatrices = new D3DXMATRIX*[uBones];

		// get each of the bone offset matrices so that we don't need to 
		//	get them later
		for (UINT i = 0; i < uBones; i++)
			pMeshContainer->pBoneOffsets[i] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
	}
	else

	{
		pMeshContainer->pSkinInfo = NULL;
		pMeshContainer->pBoneOffsets = NULL;
		pMeshContainer->pSkinMesh = NULL;
		pMeshContainer->ppFrameMatrices = NULL;
	}

	pMeshContainer->pMaterials = NULL;
	pMeshContainer->pEffects = NULL;

	//pMeshContainer->MeshData.pMesh->OptimizeInplace(
	//	D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
	//	pMeshContainer->pAdjacency,NULL,NULL,NULL);

	// Set the output mesh container to the temp one
	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;

	// Returns an HRESULT so give it the AOk result
	return S_OK;
}

HRESULT AllocMeshHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	//Convert the frame
	FRAME * pFrame = (FRAME *)pFrameToFree;

	// Delete the name
	delete [] (pFrame->Name);

	// Delete the frame
	delete (pFrame);

	// Returns an HRESULT so give it the AOk result
	return S_OK; 
}

HRESULT AllocMeshHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	//Convert to my derived struct type
	MESHCONTAINER * pMeshContainer = (MESHCONTAINER *)pMeshContainerBase;

	// if there is a name
	delete [](pMeshContainer->Name);

	//if there are materials
	delete [](pMeshContainer->pMaterials9);

	//Release the textures
	if(pMeshContainer->ppTextures)
		for(UINT i = 0; i < pMeshContainer->NumMaterials; ++i)
			util::release(pMeshContainer->ppTextures[i]);

	//if there are textures
	delete [](pMeshContainer->ppTextures);

	// if there is adjacency data
	delete [](pMeshContainer->pAdjacency) ;

	// if there are bone parts
	delete [](pMeshContainer->pBoneOffsets);

	//if there are frame matrices
	delete [](pMeshContainer->ppFrameMatrices);
	delete [](pMeshContainer->pAttributeTable);

	//if there is a copy of the mesh here
	util::release(pMeshContainer->pSkinMesh);

	//if there is a mesh
	util::release(pMeshContainer->MeshData.pMesh);

	// if there is skin information
	util::release(pMeshContainer->pSkinInfo);

	//Delete the mesh container
	delete(pMeshContainer);

	// Returns an HRESULT so give it the AOk result
	return S_OK;
}


////--------------------------------------------------------------------------------------
//// Name: AllocMeshHierarchy::CreateFrame()
//// Desc: 
////--------------------------------------------------------------------------------------
//HRESULT AllocMeshHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
//{
//	HRESULT hr = S_OK;
//	D3DXFRAME_DERIVED *pFrame;
//
//	*ppNewFrame = NULL;
//
//	pFrame = new D3DXFRAME_DERIVED;
//	if (pFrame == NULL)
//	{
//		hr = E_OUTOFMEMORY;
//		goto e_Exit;
//	}
//
//	hr = copyString(Name, &pFrame->Name);
//	if (FAILED(hr))
//		goto e_Exit;
//
//	// initialize other data members of the frame
//	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
//	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
//
//	pFrame->pMeshContainer = NULL;
//	pFrame->pFrameSibling = NULL;
//	pFrame->pFrameFirstChild = NULL;
//
//	*ppNewFrame = pFrame;
//	pFrame = NULL;
//
//e_Exit:
//	delete pFrame;
//	return hr;
//}
//
//
//
//
////--------------------------------------------------------------------------------------
//// Name: AllocMeshHierarchy::CreateMeshContainer()
//// Desc: 
////--------------------------------------------------------------------------------------
//HRESULT AllocMeshHierarchy::CreateMeshContainer(
//	LPCSTR Name, 
//	CONST D3DXMESHDATA *pMeshData,
//	CONST D3DXMATERIAL *pMaterials, 
//	CONST D3DXEFFECTINSTANCE *pEffectInstances, 
//	DWORD NumMaterials, 
//	CONST DWORD *pAdjacency, 
//	LPD3DXSKININFO pSkinInfo, 
//	LPD3DXMESHCONTAINER *ppNewMeshContainer) 
//{
//	HRESULT hr;
//	D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
//	UINT NumFaces;
//	UINT iMaterial;
//	UINT iBone, cBones;
//	LPDIRECT3DDEVICE9 pd3dDevice = NULL;
//
//	LPD3DXMESH pMesh = NULL;
//
//	*ppNewMeshContainer = NULL;
//
//	// this sample does not handle patch meshes, so fail when one is found
//	if (pMeshData->Type != D3DXMESHTYPE_MESH)
//	{
//		hr = E_FAIL;
//		goto e_Exit;
//	}
//
//	// get the pMesh interface pointer out of the mesh data structure
//	pMesh = pMeshData->pMesh;
//
//	// this sample does not FVF compatible meshes, so fail when one is found
//	if (pMesh->GetFVF() == 0)
//	{
//		hr = E_FAIL;
//		goto e_Exit;
//	}
//
//	// allocate the overloaded structure to return as a D3DXMESHCONTAINER
//	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
//	if (pMeshContainer == NULL)
//	{
//		hr = E_OUTOFMEMORY;
//		goto e_Exit;
//	}
//	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));
//
//	// make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
//	hr = copyString(Name, &pMeshContainer->Name);
//	if (FAILED(hr))
//		goto e_Exit;        
//
//	pMesh->GetDevice(&pd3dDevice);
//	NumFaces = pMesh->GetNumFaces();
//
//	// if no normals are in the mesh, add them
//	if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
//	{
//		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
//
//		// clone the mesh to make room for the normals
//		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
//			pMesh->GetFVF() | D3DFVF_NORMAL, 
//			pd3dDevice, &pMeshContainer->MeshData.pMesh );
//		if (FAILED(hr))
//			goto e_Exit;
//
//		// get the new pMesh pointer back out of the mesh container to use
//		// NOTE: we do not release pMesh because we do not have a reference to it yet
//		pMesh = pMeshContainer->MeshData.pMesh;
//
//		// now generate the normals for the pmesh
//		D3DXComputeNormals( pMesh, NULL );
//	}
//	else  // if no normals, just add a reference to the mesh for the mesh container
//	{
//		pMeshContainer->MeshData.pMesh = pMesh;
//		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
//
//		pMesh->AddRef();
//	}
//
//	// allocate memory to contain the material information.  This sample uses
//	//   the D3D9 materials and texture names instead of the EffectInstance style materials
//	pMeshContainer->NumMaterials = std::max(static_cast<DWORD>(1), NumMaterials);
//	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
//	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
//	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
//	if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
//	{
//		hr = E_OUTOFMEMORY;
//		goto e_Exit;
//	}
//
//	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);
//	memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
//
//	// if materials provided, copy them
//	if (NumMaterials > 0)            
//	{
//		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);
//
//		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
//		{
//			if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
//			{
//				/*WCHAR strTexturePath[MAX_PATH];
//				WCHAR wszBuf[MAX_PATH];
//				MultiByteToWideChar( CP_ACP, 0, pMeshContainer->pMaterials[iMaterial].pTextureFilename, -1, wszBuf, MAX_PATH );
//				wszBuf[MAX_PATH - 1] = L'\0';
//				DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, wszBuf );*/
//				if( FAILED( D3DXCreateTextureFromFile( 
//								pd3dDevice, 
//								pMeshContainer->pMaterials[iMaterial].pTextureFilename, //strTexturePath,
//								&pMeshContainer->ppTextures[iMaterial] ) ) )
//				{
//					pMeshContainer->ppTextures[iMaterial] = NULL;
//				}
//
//				// don't remember a pointer into the dynamic memory, just forget the name after loading
//				pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
//			}
//		}
//	}
//	else // if no materials provided, use a default one
//	{
//		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
//		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
//		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
//		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
//		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
//		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
//	}
//
//	// if there is skinning information, save off the required data and then setup for HW skinning
//	if (pSkinInfo != NULL)
//	{
//		// first save off the SkinInfo and original mesh data
//		pMeshContainer->pSkinInfo = pSkinInfo;
//		pSkinInfo->AddRef();
//
//		pMeshContainer->pOrigMesh = pMesh;
//		pMesh->AddRef();
//
//		// Will need an array of offset matrices to move the vertices from the figure space to the bone's space
//		cBones = pSkinInfo->GetNumBones();
//		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
//		if (pMeshContainer->pBoneOffsetMatrices == NULL)
//		{
//			hr = E_OUTOFMEMORY;
//			goto e_Exit;
//		}
//
//		// get each of the bone offset matrices so that we don't need to get them later
//		for (iBone = 0; iBone < cBones; iBone++)
//		{
//			pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
//		}
//
//		// GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
//		/*hr = GenerateSkinnedMesh( pd3dDevice, pMeshContainer );
//		if (FAILED(hr))
//			goto e_Exit;*/
//	}
//
//	*ppNewMeshContainer = pMeshContainer;
//	pMeshContainer = NULL;
//
//e_Exit:
//	util::release(pd3dDevice);
//
//	// call Destroy function to properly clean up the memory allocated 
//	if (pMeshContainer != NULL)
//	{
//		DestroyMeshContainer(pMeshContainer);
//	}
//
//	return hr;
//}
//
//
//
//
////--------------------------------------------------------------------------------------
//// Name: AllocMeshHierarchy::DestroyFrame()
//// Desc: 
////--------------------------------------------------------------------------------------
//HRESULT AllocMeshHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
//{
//	delete []( pFrameToFree->Name );
//	delete( pFrameToFree );
//	return S_OK; 
//}
//
//
//
//
////--------------------------------------------------------------------------------------
//// Name: AllocMeshHierarchy::DestroyMeshContainer()
//// Desc: 
////--------------------------------------------------------------------------------------
//HRESULT AllocMeshHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
//{
//	UINT iMaterial;
//	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
//
//	delete []( pMeshContainer->Name );
//	delete []( pMeshContainer->pAdjacency );
//	delete []( pMeshContainer->pMaterials );
//	delete []( pMeshContainer->pBoneOffsetMatrices );
//
//	// release all the allocated textures
//	if (pMeshContainer->ppTextures != NULL)
//	{
//		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
//		{
//			util::release( pMeshContainer->ppTextures[iMaterial] );
//		}
//	}
//
//	delete []( pMeshContainer->ppTextures );
//	delete []( pMeshContainer->ppBoneMatrixPtrs );
//	util::release( pMeshContainer->pBoneCombinationBuf );
//	util::release( pMeshContainer->MeshData.pMesh );
//	util::release( pMeshContainer->pSkinInfo );
//	util::release( pMeshContainer->pOrigMesh );
//	delete( pMeshContainer );
//	return S_OK;
//}
//
///*HRESULT STDMETHODCALLTYPE AllocMeshHierarchy::CreateFrame(
//	PCSTR name, 
//	D3DXFRAME **newFrame)
//{
//	D3DXFRAME_DERIVED *frame = new D3DXFRAME_DERIVED();
//
//	if (name != 0)
//		copyString(name, &frame->Name);
//	else
//		copyString("<no name>", &frame->Name);
//
//	frame->pFrameFirstChild = 0;
//	frame->pFrameSibling    = 0;
//	frame->pMeshContainer   = 0;
//	D3DXMatrixIdentity(&frame->combinedTransform);
//	D3DXMatrixIdentity(&frame->TransformationMatrix);
//
//	*newFrame = frame;
//
//	return D3D_OK;
//}
//
//HRESULT STDMETHODCALLTYPE AllocMeshHierarchy::CreateMeshContainer(
//	PCSTR name, 
//	const D3DXMESHDATA *meshData,
//	const D3DXMATERIAL *materials, 
//	const D3DXEFFECTINSTANCE *effectInstance, 
//	DWORD numMaterials, 
//	const DWORD *adjacency,
//	ID3DXSkinInfo *skinInfo, 
//	D3DXMESHCONTAINER **newMeshContainer)
//{
//	D3DXMESHCONTAINER *meshContainer = new D3DXMESHCONTAINER();
//	memset(meshContainer, 0, sizeof(D3DXMESHCONTAINER));
//	if (name != 0)
//		copyString(name, &meshContainer->Name);
//	else
//		copyString("<no name>", &meshContainer->Name);
//
//	*newMeshContainer = meshContainer;
//
//	if (skinInfo == 0 || meshData->Type != D3DXMESHTYPE_MESH)
//		return D3D_OK;
//
//	// Copy materials, and allocate memory for textures.
//	meshContainer->NumMaterials = numMaterials;
//	meshContainer->pMaterials   = new D3DXMATERIAL[numMaterials];
//	for (unsigned i = 0; i < numMaterials; i++)
//	{
//		D3DXMATERIAL *currMaterials     = meshContainer->pMaterials;
//		currMaterials[i].MatD3D         = materials[i].MatD3D;
//		currMaterials[i].MatD3D.Diffuse = materials[i].MatD3D.Diffuse;
//
//		copyString(materials->pTextureFilename, &currMaterials[i].pTextureFilename);
//	}
//
//	meshContainer->pAdjacency = 0;
//	meshContainer->pEffects   = 0;
//
//	meshContainer->MeshData.Type  = D3DXMESHTYPE_MESH;
//	meshContainer->MeshData.pMesh = meshData->pMesh;
//	meshContainer->pSkinInfo      = skinInfo;
//	meshData->pMesh->AddRef();
//	skinInfo->AddRef();
//
//	return D3D_OK;
//}
//
//HRESULT STDMETHODCALLTYPE AllocMeshHierarchy::DestroyFrame(D3DXFRAME *toDestroy)
//{
//	delete [] toDestroy->Name;
//	delete toDestroy;
//
//	return D3D_OK;
//}
//
//HRESULT STDMETHODCALLTYPE AllocMeshHierarchy::DestroyMeshContainer(D3DXMESHCONTAINER *toDestroy)
//{
//	delete [] toDestroy->Name;
//	delete [] toDestroy->pAdjacency;
//	delete [] toDestroy->pEffects;
//
//	for (unsigned i = 0; i < toDestroy->NumMaterials; i++)
//		delete [] toDestroy->pMaterials[i].pTextureFilename;
//
//	delete [] toDestroy->pMaterials;
//
//	util::release(toDestroy->MeshData.pMesh);
//	util::release(toDestroy->pSkinInfo);
//
//	delete toDestroy;
//
//	return D3D_OK;
//}
//*/