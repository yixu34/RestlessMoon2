#include <d3d9.h>
#include <d3dx9.h>

#include "util.hpp"
#include "boundingvolume.hpp"

BoundingBox::BoundingBox()
	: min(0.0f, 0.0f, 0.0f), max(0.0f, 0.0f, 0.0f)
{
}

bool BoundingBox::containsPoint(const D3DXVECTOR3 &point) const
{
	return (point.x >= min.x && point.y >= min.y && point.z >= min.z &&
			point.x <= max.x && point.y <= max.y && point.z <= max.z);
}

//////////////////////////////////////////////////////////////////////////

BoundingSphere::BoundingSphere()
	: center(0.0f, 0.0f, 0.0f), radius(0.0f)
{
}

bool BoundingSphere::containsPoint(const D3DXVECTOR3 &point) const
{
	D3DXVECTOR3 distFromCenter;
	D3DXVec3Subtract(&distFromCenter, &point, &center);

	return D3DXVec3LengthSq(&distFromCenter) <= (radius * radius);
}

//////////////////////////////////////////////////////////////////////////

BoundingCylinder::BoundingCylinder()
	: baseCenter(0.0f, 0.0f, 0.0f), radius(0.0f), height(0.0f)
{
}

//bool BoundingCylinder::containsEdge(
//	const D3DXVECTOR3 &point1, 
//	const D3DXVECTOR3 &point2) const
//{
//}

//////////////////////////////////////////////////////////////////////////

void computeBoundingBox(ID3DXMesh *mesh, BoundingBox *box)
{
	BYTE *_data;
	mesh->LockVertexBuffer(0, reinterpret_cast<void **>(&_data));
	D3DXComputeBoundingBox(
		reinterpret_cast<D3DXVECTOR3 *>(_data), 
		mesh->GetNumVertices(), 
		D3DXGetFVFVertexSize(mesh->GetFVF()), 
		&box->min, 
		&box->max);
	mesh->UnlockVertexBuffer();
}

void computeBoundingSphere(ID3DXMesh *mesh, BoundingSphere *sphere)
{
	BYTE *_data;
	mesh->LockVertexBuffer(0, reinterpret_cast<void **>(&_data));
	D3DXComputeBoundingSphere(
		reinterpret_cast<D3DXVECTOR3 *>(_data), 
		mesh->GetNumVertices(), 
		D3DXGetFVFVertexSize(mesh->GetFVF()), 
		&sphere->center, 
		&sphere->radius);
	mesh->UnlockVertexBuffer();
}

void computeMaxBoundingBox(
	BoundingBox *result, 
	BoundingBox *firstBox, 
	BoundingBox *secondBox)
{
	// First box min
    float min1x = firstBox->min.x;
    float min1y = firstBox->min.y;
    float min1z = firstBox->min.z;

	// First box max
    float max1x = firstBox->max.x;
    float max1y = firstBox->max.y;
    float max1z = firstBox->max.z;

	// Second box min
	float min2x = secondBox->min.x;
	float min2y = secondBox->min.y;
	float min2z = secondBox->min.z;

	// Second box max
	float max2x = secondBox->max.x;
	float max2y = secondBox->max.y;
	float max2z = secondBox->max.z;

	float resultMinX = std::min(min1x, std::min(max1x, std::min(min2x, max2x)));
	float resultMinY = std::min(min1y, std::min(max1y, std::min(min2y, max2y)));
	float resultMinZ = std::min(min1z, std::min(max1z, std::min(min2z, max2z)));

	float resultMaxX = std::max(min1x, std::max(max1x, std::max(min2x, max2x)));
	float resultMaxY = std::max(min1y, std::max(max1y, std::max(min2y, max2y)));
	float resultMaxZ = std::max(min1z, std::max(max1z, std::max(min2z, max2z)));

    result->min = D3DXVECTOR3(resultMinX, resultMinY, resultMinZ);
    result->max = D3DXVECTOR3(resultMaxX, resultMaxY, resultMaxZ);
}

void computeMaxBoundingSphere(
	BoundingSphere *result, 
	BoundingSphere *firstSphere, 
	BoundingSphere *secondSphere)
{
	D3DXVECTOR3 midPoint    = (secondSphere->center + firstSphere->center) / 2;
	D3DXVECTOR3 midPointVec = midPoint - firstSphere->center;
	float midPointDist      = D3DXVec3Length(&midPointVec);
	float newRadius         = midPointDist + 
							  std::max(firstSphere->radius, secondSphere->radius);

	result->radius = newRadius;
	result->center = midPoint;
}
