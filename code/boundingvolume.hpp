#ifndef BOUNDINGVOLUME_HPP
#define BOUNDINGVOLUME_HPP

#ifdef min
#	undef min
#endif

#ifdef max
#	undef max
#endif

//
// Bounding volume classes for boxes and spheres.
//

struct BoundingBox
{
	BoundingBox();

	bool containsPoint(const D3DXVECTOR3 &point) const;

	D3DXVECTOR3 min;
	D3DXVECTOR3 max;
};

//////////////////////////////////////////////////////////////////////////

struct BoundingSphere
{
	BoundingSphere();

	bool containsPoint(const D3DXVECTOR3 &point) const;

	D3DXVECTOR3 center;
	float radius;
};

//////////////////////////////////////////////////////////////////////////

struct BoundingCylinder
{
	BoundingCylinder();

	//bool containsEdge(const D3DXVECTOR3 &point1, const D3DXVECTOR3 &point2) const;

	D3DXVECTOR3 baseCenter;
	float radius;
	float height;
};

//////////////////////////////////////////////////////////////////////////

struct ID3DXMesh;
void computeBoundingBox(
	ID3DXMesh *mesh, 
	BoundingBox *box);

void computeBoundingSphere(
	ID3DXMesh *mesh, 
	BoundingSphere *sphere);

// Computes the bounding box that bounds both firstBox and secondBox.
void computeMaxBoundingBox(
	BoundingBox *result, 
	BoundingBox *firstBox, 
	BoundingBox *secondBox);

// Computes the bounding sphere that bounds both firstShere and secondSphere.
void computeMaxBoundingSphere(
	BoundingSphere *result, 
	BoundingSphere *firstSphere, 
	BoundingSphere *secondSphere);

#endif
