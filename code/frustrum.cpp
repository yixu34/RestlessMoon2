#include <d3d9.h>
#include <d3dx9.h>

#include "frustrum.hpp"
#include "graphics.hpp"

Frustrum::Frustrum()
{
	_fovY        = D3DX_PI * 0.5f;
	_aspectRatio = graphics->getScreenWidth() / graphics->getScreenHeight();
	_nearDist    = 1.0f;
	_farDist     = 1000.0f;

    _planes.resize(num_planes);

    // Treating the near and far planes as quadrilaterals, hence the 4 points.
    _nearPoints.resize(4);
    _farPoints.resize(4);
}

Frustrum::~Frustrum()
{
}

void Frustrum::set(
	float fovY, 
	float aspectRatio, 
	float nearDist, 
	float farDist)
{
	_fovY        = fovY;
	_aspectRatio = aspectRatio;
	_nearDist    = nearDist;
	_farDist     = farDist;

    float tanHalfFovY = tan(_fovY / 2);
    float nearY       = _nearDist * tanHalfFovY;
    float nearX       = _aspectRatio * nearY;

    // Vertices are labeled 0-3, starting from bottom left, going clockwise.
    _nearPoints[0] = D3DXVECTOR3(-nearX, -nearY, nearDist);
    _nearPoints[1] = D3DXVECTOR3(-nearX,  nearY, nearDist);
    _nearPoints[2] = D3DXVECTOR3( nearX,  nearY, nearDist);
    _nearPoints[3] = D3DXVECTOR3( nearX, -nearY, nearDist);

    float farY     = _farDist * tanHalfFovY;
    float farX     = _aspectRatio * farY;
    _farPoints[0]  = D3DXVECTOR3(-farX, -farY, farDist);
    _farPoints[1]  = D3DXVECTOR3(-farX,  farY, farDist);
    _farPoints[2]  = D3DXVECTOR3( farX,  farY, farDist);
    _farPoints[3]  = D3DXVECTOR3( farX, -farY, farDist);

    resetPlanes();
}

void Frustrum::resetPlanes()
{
    static const D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);

    D3DXPLANE nearPlane;
    D3DXPlaneFromPoints(
        &nearPlane, 
        &_nearPoints[2], 
        &_nearPoints[1], 
        &_nearPoints[0]);
    _planes[plane_near] = nearPlane;


    D3DXPLANE farPlane;
    D3DXPlaneFromPoints(
        &farPlane, 
        &_farPoints[0], 
        &_farPoints[1], 
        &_farPoints[2]);
    _planes[plane_far] = farPlane;


    D3DXPLANE topPlane;
    D3DXPlaneFromPoints(
        &topPlane, 
        &_farPoints[2],
        &_farPoints[1], 
        &origin);
    _planes[plane_top] = topPlane;


    D3DXPLANE bottomPlane;
    D3DXPlaneFromPoints(
        &bottomPlane, 
        &_farPoints[0],
        &_farPoints[3], 
        &origin);
    _planes[plane_bottom] = bottomPlane;


    D3DXPLANE leftPlane;
    D3DXPlaneFromPoints(
        &leftPlane, 
        &origin, 
        &_farPoints[1], 
        &_farPoints[0]);
    _planes[plane_left] = leftPlane;


    D3DXPLANE rightPlane;
    D3DXPlaneFromPoints(
        &rightPlane, 
        &origin, 
        &_farPoints[3], 
        &_farPoints[2]);
    _planes[plane_right] = rightPlane;
}

bool Frustrum::isInside(const BoundingBox &box) const
{
	// TODO:  Implement this.
	return false;
}

bool Frustrum::isInside(const BoundingSphere &sphere) const
{
    // TODO:  Check this.
    for (PlaneList::const_iterator it = _planes.begin(); 
		 it != _planes.end(); it++)
    {
        const D3DXPLANE &currPlane = *it;

        if (D3DXPlaneDotCoord(&currPlane, &sphere.center) < -sphere.radius)
            return false;
    }
    return true;
}

bool Frustrum::isInside(const D3DXVECTOR3 &point) const
{
    for (PlaneList::const_iterator it = _planes.begin(); 
		 it != _planes.end(); it++)
    {
        const D3DXPLANE &currPlane = *it;

        // The point is NOT inside if it is on the reverse 
        // (non-normal) side of any plane,
        if (D3DXPlaneDotCoord(&currPlane, &point) < 0.0f)
            return false;
    }

    return true;
}
