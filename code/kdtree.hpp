#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <string>
#include <vector>

#include "entity.hpp"
#include "renderable.hpp"
#include "boundingvolume.hpp"
#include "ray.hpp"

struct Triangle;
typedef std::vector<Triangle> TriangleList;
typedef std::vector<DWORD> IndexList;

class KDTree
{
public:
	KDTree();
	~KDTree();

	void build();
	void clear();
	void addObject(const RenderablePtr &renderable);
	void collideWithEntity(EntityPtr entity);

	// Returns true if the ray intersects the kd-tree at all.
	// Don't bother normalizing the ray's direction, because 
	// the ray's length is used as a cutoff distance, beyond which
	// intersections are ignored.
	bool doesRayIntersect(const Ray &ray) const;

private:
	class KDPlane;
	class KDNode;

	enum kd_axis
	{
		x_axis,
		y_axis,
		z_axis,
		no_axis,
	};

	static bool intersectsWithUnitSphere(
		D3DXVECTOR3 center,
		D3DXVECTOR3 origin,
		D3DXVECTOR3 destination);

	static D3DXVECTOR3 projectPointOntoLine(const D3DXVECTOR3 &point,
											const D3DXVECTOR3 &origin,
											const D3DXVECTOR3 &destination);

	static bool repositionEntity(
		const D3DXVECTOR3 &center, 
		const D3DXVECTOR3 &origin, 
		const D3DXVECTOR3 &destination, 
		EntityPtr entity);

	void getIndexVector(ID3DXMesh *mesh, IndexList &indices);

	void getTriangleVector(
		ID3DXMesh *mesh, 
		const IndexList &indices, 
		TriangleList &triangles);

	void addPendingTriangles(const TriangleList &trianglesToAdd);

	void subdivide(
		KDNode *currNode, 
		kd_axis currAxis, 
		int currHeight);

	bool shouldSubdivide(KDNode *currNode, int currHeight);
	kd_axis getNextAxis(kd_axis currAxis) const;
	bool splitNode(KDNode *currNode);

	void collideWithEntity(EntityPtr entity, int depth);

	static const int maxHeight;
	static const int maxSurfacesPerNode;
	static const int maxSlideRecursion;
	static const float coeffRestitution;
	static const float coeffFriction;

	KDNode *_root;
	typedef std::set<RenderablePtr> RenderableSet;
	RenderableSet _pendingObjects;
	TriangleList _pendingTriangles;
};

extern KDTree kdTree;

//////////////////////////////////////////////////////////////////////////

class KDTree::KDPlane
{
public:
	KDPlane(float axisValue = 0.0f, kd_axis axis = no_axis);
	~KDPlane();

	void setFromBox(kd_axis axis, const BoundingBox &box);

	enum tri_plane_result
	{
		tri_plane_intersect = 0, 
		tri_plane_negative  = -1, 
		tri_plane_positive  = 1,
		tri_plane_error,
	};

	tri_plane_result intersectTrianglePlane(const Triangle &triangle);

	float   _axisValue;
	kd_axis _axis;
};

//////////////////////////////////////////////////////////////////////////

class KDTree::KDNode
{
public:
	KDNode(const TriangleList &triangles = TriangleList());
	~KDNode();

	bool isLeaf() const;
	void addTriangle(const Triangle &triangle);
	BoundingBox getBoundingBox() const;
	void getNearestCollision(bool &collisionHappened,
							 float &collisionDistance,
							 D3DXVECTOR3 &collisionPoint,
							 const D3DXVECTOR3 &position,
							 float radius,
							 float inverseRadius,
							 float magnitude,
							 const D3DXVECTOR3 &direction,
							 const D3DXVECTOR3 &minima,
							 const D3DXVECTOR3 &maxima);


	bool doesRayIntersect(const Ray &ray, const BoundingBox &rayBoundingBox) const;

private:
	bool collideTriangleWithEntity(Triangle & triangle, EntityPtr entity);
	
public:
	TriangleList _triangles;
	KDNode *_left;
	KDNode *_right;
	KDPlane _plane;
};

/** Find the point a distance t along a specified ray **/
D3DXVECTOR3 pointOnRay(float t, const Ray &ray);

/** A ray-triangle intersection test **/
bool rayTriangleIntersect(const Ray &ray, 
						  const Triangle &triangle);

/** A ray-plane intersection test **/
bool rayPlaneIntersect(float &t,
					   const Ray &ray,
					   const D3DXVECTOR3 &planeOrigin,
					   const D3DXVECTOR3 &planeNormal);

/** A ray-ellipse intersection test **/
bool raySphereIntersect(float &t,
						D3DXVECTOR3 rayOrigin,
						D3DXVECTOR3 rayDirection,
						const D3DXVECTOR3& spherePosition,
						float inverseRadius);

/** Returns the closest point on the boundary of a triangle
     to some point **/
void closestOnBoundary(D3DXVECTOR3 *pointOut,
							  const D3DXVECTOR3 &point,
							  const Triangle& triangle);

/** Returns the closest point on a segment to some point **/
/** Returns the closest point on a segment to some point **/
inline
D3DXVECTOR3 closestOnSegment(const D3DXVECTOR3 &point,
							 const D3DXVECTOR3 &segmentOrigin,
							 const D3DXVECTOR3 &segmentDirection,
							 float segmentLength)
{
	// Find the parameter of projection of the point onto the line,
	//  call it coincidence
	D3DXVECTOR3 pointMinusOrigin;
	D3DXVec3Subtract(&pointMinusOrigin,&point,&segmentOrigin);
	float coincidence = D3DXVec3Dot(&segmentDirection,&pointMinusOrigin);

	// Set the parameter to lie on the segment
	coincidence = util::clamp(coincidence,0.0f,segmentLength);

	// Set the point on the ray defined by the coincidence parameter
	return segmentOrigin + (segmentDirection*coincidence);
}

/** Get the barycentric coordinates of point in a triangle **/
D3DXVECTOR3 barycentric(const D3DXVECTOR3 &point,
						const Triangle &triangle);

/** Componentwise division **/
D3DXVECTOR3 componentDivision(const D3DXVECTOR3 &dividend,
							  const D3DXVECTOR3 &divisor);

/** Componentwise multiplication **/
D3DXVECTOR3 componentMultiplication(const D3DXVECTOR3 &vector1,
									const D3DXVECTOR3 &vector2);

#endif
