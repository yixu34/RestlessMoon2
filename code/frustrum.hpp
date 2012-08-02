#ifndef FRUSTRUM_HPP
#define FRUSTRUM_HPP

//
// Viewing frustrum used for culling in the scene graph.
//

#include <vector>

#include "boundingvolume.hpp"

struct D3DXVECTOR3;

class Frustrum
{
public:
	Frustrum();
	~Frustrum();

	void set(
		float fovY, 
		float aspectRatio, 
		float nearDist, 
		float farDist);

	bool isInside(const BoundingBox &box) const;
	bool isInside(const BoundingSphere &sphere) const;
	bool isInside(const D3DXVECTOR3 &point) const;

private:
	enum
	{
		plane_near, 
		plane_far, 
		plane_top, 
		plane_bottom, 
		plane_left, 
		plane_right, 
		num_planes, // Not used - this is an array/loop sentinel value.
	};

    void resetPlanes();

    // NOTE:  All planes are specified in counter-clockwise order, thus they 
	// all have INWARD facing normals!
	typedef std::vector<D3DXPLANE> PlaneList;
	PlaneList _planes;

	// Arrays of four points defining rectangles for the near and far planes, 
	// respectively.
    typedef std::vector<D3DXVECTOR3> PointList;
    PointList _nearPoints;
    PointList _farPoints;

	float _fovY;
	float _aspectRatio;
	float _nearDist;
	float _farDist;
};

#endif
