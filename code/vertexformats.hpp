#ifndef VERTEXFORMATS_HPP
#define VERTEXFORMATS_HPP

//
// Custom flexible vertex format structures.
//

#include <d3d9.h>
#include <d3dx9.h>

// 
// For world geometry vertices.
//
struct CollisionVertex
{
	CollisionVertex()
	{
	}

	CollisionVertex(float x, float y, float z)
	{
		_position = D3DXVECTOR3(x, y, z);
	}

	D3DXVECTOR3 _position;

	static const DWORD FVF = D3DFVF_XYZ;
};


//
// For colored vertices specified in screen coordinates.
//
struct ColoredScreenVertex
{
	ColoredScreenVertex(
		float x, 
		float y, 
		float z, 
		float rhw, 
		DWORD color)
	{
		this->x   = x;
		this->y   = y;
		this->z   = z;
		this->rhw = rhw;

		this->color = color;
	}

	float x, y, z, rhw;
	DWORD color;

	static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
};

//////////////////////////////////////////////////////////////////////////

//
// For texture mapped vertices in screen coordinates.
//
struct SpriteVertex
{
	SpriteVertex(
		float x, float y, float z, float rhw, 
		float u, float v)
	{
		this->x   = x;
		this->y   = y;
		this->z   = z;
		this->rhw = rhw;

		this->u = u;
		this->v = v;
	}

	float x, y, z, rhw;
	float u, v;

	static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
};

//////////////////////////////////////////////////////////////////////////

//
// Used for texture mapped vertices with normals.
//
struct MeshVertex
{
	MeshVertex(
		float x,  float y, float z, 
		float nx, float ny, float nz, 
		float u,  float v)
    {
        this->x = x;
        this->y = y;
        this->z = z;

        this->nx = nx;
        this->ny = ny;
        this->nz = nz;

        this->u = u;
        this->v = v;
    }

    float x, y, z;
    float nx, ny, nz;  // normal coordinates
    float u, v;

    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

//////////////////////////////////////////////////////////////////////////

//
// Used for 3D that uses precomputed lighting and doesn't need a normal.
//
struct TextureVertex
{
	TextureVertex(
		float x, float y, float z, 
		float u, float v)
	{
		this->x = x;
		this->y = y;
		this->z = z;

		this->u = u;
		this->v = v;
	}

	float x, y, z;
	float u, v;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
};

//
// Used for planespace.
//
struct PlanespaceVertex
{
	float x,y,z;
	float nx,ny,nz;
	DWORD color;
	float u,v;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;
};

#endif
