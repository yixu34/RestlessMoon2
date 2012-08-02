#include "skinnedmesh.hpp"
#include "graphics.hpp"
#include "timer.hpp"
#include "common.hpp"
#include "util.hpp"
#include "entity.hpp"

#include "renderer.hpp"

SkinnedMeshCache skinnedMeshCache;

SkinnedMesh::SkinnedMesh()
{
	m_pFrameRoot = NULL;			
	m_pBoneMatrices = NULL;			
	m_uMaxBones = 0;
	m_pAnimController = NULL;
	m_pFirstMesh = NULL;
}

SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::unload()
{
	//Delete Animation Controller
	util::release(m_pAnimController);

	//if there is a frame hierarchyo
	if(m_pFrameRoot)
	{
		//Allocation class
		AllocMeshHierarchy Alloc;
		D3DXFrameDestroy(m_pFrameRoot, &Alloc);
		m_pFrameRoot = NULL;
	}

	//Delete the bones
	delete [] (m_pBoneMatrices);
	m_pBoneMatrices = 0;
}

bool SkinnedMesh::loadData(const std::string &filename)
{
	//Allocation class
	AllocMeshHierarchy Alloc;

	//Load the mesh
	if(FAILED(D3DXLoadMeshHierarchyFromX(
		filename.c_str(),		// File load
		D3DXMESH_MANAGED,	// Load Options
		graphics->getDevice(),		// D3D Device
		&Alloc,				// Hierarchy allocation class
		NULL,				// NO Effects
		&m_pFrameRoot,		// Frame hierarchy
		&m_pAnimController)))// Animation Controller
	{
		ERRORMSG("Couldn't load skinned mesh file!");
		return false;
	}

	if(m_pFrameRoot)
	{
		//Set the bones up
		setupBoneMatrices((FRAME *)m_pFrameRoot, NULL);

		//Setup the bone matrices array 
		m_pBoneMatrices  = new D3DXMATRIX[m_uMaxBones];
		ZeroMemory(m_pBoneMatrices, sizeof(D3DXMATRIX)*m_uMaxBones);
	}

	return true;
}

void SkinnedMesh::setupBoneMatrices(FRAME *frame, LPD3DXMATRIX parentMatrix)
{
	MESHCONTAINER *pMesh = (MESHCONTAINER *)frame->pMeshContainer;

	//Set up the bones on the mesh
	if(pMesh)
	{
		if(!m_pFirstMesh)
			m_pFirstMesh = pMesh;

		// if there is a skinmesh, then setup the bone matrices
		if(pMesh->pSkinInfo)
		{
			//Create a copy of the mesh
			pMesh->MeshData.pMesh->CloneMeshFVF(
				D3DXMESH_MANAGED, 
				pMesh->MeshData.pMesh->GetFVF(), 
				graphics->getDevice(), 
				&pMesh->pSkinMesh);

			if(m_uMaxBones < pMesh->pSkinInfo->GetNumBones())
			{
				//Get the number of bones
				m_uMaxBones = pMesh->pSkinInfo->GetNumBones();
			}

			FRAME *pTempFrame = NULL;

			//For each bone 
			for (UINT i = 0; i < pMesh->pSkinInfo->GetNumBones(); i++)
			{   
				// Find the frame
				pTempFrame = (FRAME *)D3DXFrameFind(m_pFrameRoot, 
					pMesh->pSkinInfo->GetBoneName(i));

				//set the bone part
				pMesh->ppFrameMatrices[i] = &pTempFrame->matCombined;
			}
		}
	}

	//Check your Sister
	if(frame->pFrameSibling)
		setupBoneMatrices((FRAME *)frame->pFrameSibling, parentMatrix);

	//Check your Son
	if(frame->pFrameFirstChild)
		setupBoneMatrices((FRAME *)frame->pFrameFirstChild, &frame->matCombined);
}

void SkinnedMesh::render()
{
	renderSubset(0);
}

void SkinnedMesh::renderSubset(int subset)
{
	MESHCONTAINER *pMesh = m_pFirstMesh;

	//While there is a mesh try to draw it
	while(pMesh)
	{
		//Select the mesh to draw
		LPD3DXMESH pDrawMesh = 
			(pMesh->pSkinInfo)? pMesh->pSkinMesh : pMesh->MeshData.pMesh;

		pDrawMesh->DrawSubset(subset);

		//Go to the next one
		pMesh = (MESHCONTAINER *)pMesh->pNextMeshContainer;
	}
}

void SkinnedMesh::renderFrames(int subset)
{
	//Update the frame hierarchy
	if(m_pFrameRoot)
	{
		updateFrameMatrices((FRAME *)m_pFrameRoot, NULL);

		MESHCONTAINER *pMesh = m_pFirstMesh;
		if(pMesh)
		{
			if(pMesh->pSkinInfo)
			{
				UINT Bones = pMesh->pSkinInfo->GetNumBones();
				for (UINT i = 0; i < Bones; ++i)
				{	
					D3DXMatrixMultiply(
						&m_pBoneMatrices[i],//out
						&pMesh->pBoneOffsets[i], 
						pMesh->ppFrameMatrices[i]
					);
				}

				// Lock the meshes' vertex buffers
				void *SrcPtr, *DestPtr;
				pMesh->MeshData.pMesh->LockVertexBuffer(0, (void**)&SrcPtr);
				pMesh->pSkinMesh->LockVertexBuffer(0, (void**)&DestPtr);

				// Update the skinned mesh using provided transformations
				pMesh->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, SrcPtr, DestPtr);

				// Unlock the meshes vertex buffers
				pMesh->pSkinMesh->UnlockVertexBuffer();
				pMesh->MeshData.pMesh->UnlockVertexBuffer();
			}
		}
	}

	renderFrame(static_cast<FRAME *>(m_pFrameRoot), subset);
}

void SkinnedMesh::renderFrame(FRAME *frame, int subset)
{

	MESHCONTAINER *pMesh = (MESHCONTAINER *)frame->pMeshContainer;

	//While there is a mesh try to draw it
	while(pMesh)
	{
		//Select the mesh to draw
		LPD3DXMESH pDrawMesh = 
			(pMesh->pSkinInfo) ? pMesh->pSkinMesh : pMesh->MeshData.pMesh;

		//Draw each mesh subset with correct materials and texture
		//for (DWORD i = 0; i < pMesh->NumMaterials; ++i)
		pDrawMesh->DrawSubset(subset);

		//Go to the next one
		pMesh = (MESHCONTAINER *)pMesh->pNextMeshContainer;
	}

	//Check your Sister
	if(frame->pFrameSibling)
		renderFrame((FRAME *)frame->pFrameSibling, subset);

	//Check your Son
	if(frame->pFrameFirstChild)
		renderFrame((FRAME *)frame->pFrameFirstChild, subset);
}

void SkinnedMesh::timepass()
{
	////Update the frame hierarchy
	//if(m_pFrameRoot)
	//{
	//	updateFrameMatrices((FRAME *)m_pFrameRoot, NULL);

	//	MESHCONTAINER *pMesh = m_pFirstMesh;
	//	if(pMesh)
	//	{
	//		if(pMesh->pSkinInfo)
	//		{
	//			UINT Bones = pMesh->pSkinInfo->GetNumBones();
	//			for (UINT i = 0; i < Bones; ++i)
	//			{	
	//				D3DXMatrixMultiply(
	//					&m_pBoneMatrices[i],//out
	//					&pMesh->pBoneOffsets[i], 
	//					pMesh->ppFrameMatrices[i]
	//				);
	//			}

	//			// Lock the meshes' vertex buffers
	//			void *SrcPtr, *DestPtr;
	//			pMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&SrcPtr);
	//			pMesh->pSkinMesh->LockVertexBuffer(0, (void**)&DestPtr);

	//			// Update the skinned mesh using provided transformations
	//			pMesh->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, SrcPtr, DestPtr);

	//			// Unlock the meshes vertex buffers
	//			pMesh->pSkinMesh->UnlockVertexBuffer();
	//			pMesh->MeshData.pMesh->UnlockVertexBuffer();
	//		}
	//	}
	//}
}

void SkinnedMesh::updateFrameMatrices(FRAME *frame, LPD3DXMATRIX parentMatrix)
{
	//Parent check
	if (parentMatrix)
	{
		D3DXMatrixMultiply(
			&frame->matCombined, 
			&frame->TransformationMatrix, 
			parentMatrix);
	}
	else
		frame->matCombined = frame->TransformationMatrix;

	//Do the kid too
	if (frame->pFrameSibling)
	{
		updateFrameMatrices((FRAME *)frame->pFrameSibling, parentMatrix);
	}

	//make sure you get the first kid
	if (frame->pFrameFirstChild)
	{
		updateFrameMatrices(
			(FRAME *)frame->pFrameFirstChild, 
			&frame->matCombined);
	}
}

bool SkinnedMesh::isStatic() const
{
	return false;
}

ID3DXMesh *SkinnedMesh::getMeshData() const
{
	MESHCONTAINER *pMesh = (MESHCONTAINER *)m_pFrameRoot->pMeshContainer;

	//Select the mesh to draw
	LPD3DXMESH pDrawMesh = pMesh->MeshData.pMesh;
	return pDrawMesh;
}

ID3DXAnimationController *SkinnedMesh::getNewController() const
{
	// TODO:  Check these parameters.
	ID3DXAnimationController *copy;
	m_pAnimController->CloneAnimationController(
		m_pAnimController->GetMaxNumAnimationOutputs(), 
		m_pAnimController->GetMaxNumAnimationSets(), 
		m_pAnimController->GetMaxNumTracks(), 
		m_pAnimController->GetMaxNumEvents(), 
		&copy);

	return copy;
}