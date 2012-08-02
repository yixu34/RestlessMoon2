#include <limits>
#include <cmath>
#include <d3dx9.h>
#include <algorithm>

#include "kdtree.hpp"
#include "graphics.hpp"
#include "common.hpp"
#include "vertexformats.hpp"
#include "util.hpp"

KDTree kdTree;

const int KDTree::maxHeight          = 17;
const int KDTree::maxSurfacesPerNode = 15;

// The collideWithWorld function is recursive.  In practice
//   I have encountered infinite recursions, probably brought
//   on by another bug.  For speed purposes, however, I thought
//   it best to leave this cap in here.  Candidate for a console
//   variable.
const int KDTree::maxSlideRecursion  = 4;

// The coefficient of friction describes how an object slides
//   on a surface.  Increasing this, I believe, will cause
//   a player to move more slowly.  Best used judiciously.
//   Make it an entity trait or a console variable.
//   TODO: static and dynamic coefficients of friction?
const float KDTree::coeffFriction     = 0.0f;

struct Triangle
{
	Triangle()
		: _v0(CollisionVertex()), _v1(CollisionVertex()), _v2(CollisionVertex())
	{}
	Triangle(const CollisionVertex &v0, const CollisionVertex &v1, const CollisionVertex &v2)
		: _v0(v0), _v1(v1), _v2(v2)
	{
		D3DXVECTOR3 vec21 = v1._position - v0._position;
		D3DXVECTOR3 vec31 = v2._position - v0._position;
		D3DXVec3Cross(&_normal, &vec21, &vec31);
		D3DXVec3Normalize(&_normal, &_normal);

		// bMinusA
		D3DXVec3Subtract(&_edge0,&v1._position,&v0._position);
		_edgeLength0 = D3DXVec3Length(&_edge0);
		D3DXVec3Normalize(&_edgeDirection0,&_edge0);

		// cMinusB
		D3DXVec3Subtract(&_edge1,&v2._position,&v1._position);
		_edgeLength1 = D3DXVec3Length(&_edge1);
		D3DXVec3Normalize(&_edgeDirection1,&_edge1);

		// aMinusC
		D3DXVec3Subtract(&_edge2,&v0._position,&v2._position);
		_edgeLength2 = D3DXVec3Length(&_edge2);
		D3DXVec3Normalize(&_edgeDirection2,&_edge2);

		D3DXVec3Cross(&_modNormal,&_edge0,&_edge2);
		float _negInverseNormSq = -1.0f/D3DXVec3LengthSq(&_modNormal);
		_modNormal *= _negInverseNormSq;
	}

	void getMinMax(
		float &minX, float &minY, float &minZ, 
		float &maxX, float &maxY, float &maxZ) const
	{
		minX = std::min(_v0._position.x, std::min(_v1._position.x, _v2._position.x));
		minY = std::min(_v0._position.y, std::min(_v1._position.y, _v2._position.y));
		minZ = std::min(_v0._position.z, std::min(_v1._position.z, _v2._position.z));

		maxX = std::max(_v0._position.x, std::max(_v1._position.x, _v2._position.x));
		maxY = std::max(_v0._position.y, std::max(_v1._position.y, _v2._position.y));
		maxZ = std::max(_v0._position.z, std::max(_v1._position.z, _v2._position.z));
	}

	CollisionVertex _v0, _v1, _v2;
	D3DXVECTOR3 _normal;
	D3DXVECTOR3 _modNormal;
	D3DXVECTOR3 _edge0, _edge1, _edge2;
	D3DXVECTOR3 _edgeDirection0, _edgeDirection1, _edgeDirection2;
	float _edgeLength0, _edgeLength1, _edgeLength2;
};

/************************************************************************/
/* Geometry Computations                                                */
/************************************************************************/

/** Find the point a distance t along a specified ray **/
D3DXVECTOR3 pointOnRay(float t, const Ray &ray)
{
	return ray.origin + t*ray.direction;
}

/** A ray-triangle intersection test **/
bool rayTriangleIntersect(const Ray &ray, 
						  const Triangle &triangle)
{
	//
	// This intersection code is haxxed from the CS465 ray2 programming
	// assignment, ftw.
	//

	float rayLength = D3DXVec3Length(&ray.direction);
	D3DXVECTOR3 rayNormalized;
	D3DXVec3Normalize(&rayNormalized, &ray.direction);


	// Rename the components of each vertex.
    float v0x = triangle._v0._position.x;
    float v0y = triangle._v0._position.y;
    float v0z = triangle._v0._position.z;
    float v1x = triangle._v1._position.x;
    float v1y = triangle._v1._position.y;
    float v1z = triangle._v1._position.z;
    float v2x = triangle._v2._position.x;
    float v2y = triangle._v2._position.y;
    float v2z = triangle._v2._position.z;

    // Compute elements of the triangle ray matrix.
    float A = v0x - v1x;
    float B = v0y - v1y;
    float C = v0z - v1z;
    float D = v0x - v2x;
    float E = v0y - v2y;
    float F = v0z - v2z;

    // Rename ray directions.
   /* float G = ray.direction.x;
    float H = ray.direction.y;
    float I = ray.direction.z;*/
	float G = rayNormalized.x;
	float H = rayNormalized.y;
	float I = rayNormalized.z;

    // Compute the factors in the numberator of the beta coordinate.
    float EIHF = (E * I) - (H * F);
    float GFDI = (G * F) - (D * I);
    float DHEG = (D * H) - (E * G);

    // Inverse of the denominator factor.
    float inverseDenominator = 1.0f / ((A * EIHF) + (B * GFDI) + (C * DHEG));

    // Compute the direction from the ray origin to the first vertex.
    float J = v0x - ray.origin.x;
    float K = v0y - ray.origin.y;
    float L = v0z - ray.origin.z;

    // Compute the beta coordinate.
    float beta = inverseDenominator * ((J * EIHF) + (K * GFDI) + (L * DHEG));

    // If beta is out of range, then there's no intersection.
    if (beta < 0 || beta > 1)
        return false;

    // Compute the factors of the numerator of the gamma coordinate.
    float AKJB = (A * K) - (J * B);
    float JCAL = (J * C) - (A * L);
    float BLKC = (B * L) - (K * C);

    // Calculate the other barycentric coordinate.
    float gamma = inverseDenominator * ((I * AKJB) + (H * JCAL) + (G * BLKC));

    // If the gamma coordinate is out of range, then there's no intersection.
    if (gamma < 0 || beta + gamma > 1)
        return false;

    // Calculate the ray t value.
    float t = -inverseDenominator * ((F * AKJB) + (E * JCAL) + (D * BLKC));

    // Return true only if we're on the correct half-life of the ray, 
    // and if the intersection isn't past the ray direction's length.
    //return (t >= 0.0f && t <= D3DXVec3Length(&ray.direction));
	return (t >= 0.0f && t < rayLength);
}


/** A ray-plane intersection test **/
bool rayPlaneIntersect(float &t,
					   const Ray &ray,
					   const D3DXVECTOR3 &planeOrigin,
					   const D3DXVECTOR3 &planeNormal)
{
	// t = [(planeOrigin-rayOrigin) dot planeNormal]/(rayDirection dot planeNormal)
	float numerator = D3DXVec3Dot(&planeNormal,&planeOrigin)-D3DXVec3Dot(&planeNormal,&ray.origin);
	float denominator = D3DXVec3Dot(&planeNormal,&ray.direction);
	if(denominator == 0.0f) return false;
	t = numerator/denominator;
	return (t > -util::EPSILON);
}

/** A ray-ellipse intersection test **/
bool raySphereIntersect(float &t,
						 D3DXVECTOR3 rayOrigin,
						 D3DXVECTOR3 rayDirection,
						 const D3DXVECTOR3& spherePosition,
						 float inverseRadius)
{
	// Find the ray origin in the space where the
	//  ellipse is a unit sphere centered at the origin
	rayOrigin -= spherePosition;
	rayOrigin *= inverseRadius;

	// Find the ray direction in the space where the
	//  ellipse is a unit sphere centered at the origin
	rayDirection *= inverseRadius;

	// intersection = [-(rayDirection dot rayOrigin) - sqrt[(rayDirection dot rayOrigin)^2 -
	//                                                      (rayDirection dot rayDirection)*
	//                                                      (rayOrigin dot rayOrigin - 1)]]
	//                over (rayDirection dot rayDirection)
	float dDotP = D3DXVec3Dot(&rayDirection,&rayOrigin),
		  dDotD = D3DXVec3Dot(&rayDirection,&rayDirection),
		  pDotP = D3DXVec3Dot(&rayOrigin,&rayOrigin);
	float discriminant = dDotP*dDotP - dDotD*(pDotP - 1.0f);
	if(discriminant < 0) return false;
	t = (-dDotP - std::sqrt(discriminant))/dDotD;
	return true;

}

/** Returns the closest point on the boundary of a triangle
     to some point **/
void closestOnBoundary(D3DXVECTOR3 *pointOut,
							  const D3DXVECTOR3 &point,
							  const Triangle& triangle)
{
	// Find the segment closest points
	D3DXVECTOR3 p1 = closestOnSegment(point,triangle._v0._position,triangle._edgeDirection0,triangle._edgeLength0),
		        p2 = closestOnSegment(point,triangle._v1._position,triangle._edgeDirection1,triangle._edgeLength1),
				p3 = closestOnSegment(point,triangle._v2._position,triangle._edgeDirection2,triangle._edgeLength2);
	// Find the square distances to these points
	D3DXVECTOR3 v0 = p1-point, v1 = p2-point, v2 = p3-point;
	float dsq1 = D3DXVec3LengthSq(&v0),
		  dsq2 = D3DXVec3LengthSq(&v1),
		  dsq3 = D3DXVec3LengthSq(&v2);
	// If v0's distance is shortest, then p1 is closest
	if(dsq1 <= dsq2 && dsq1 <= dsq3) {
		*pointOut = p1;
	// If v1's distance is shortest, the p2 is closest
	} else if(dsq2 <= dsq3) {
		*pointOut = p2;
	} else {
		*pointOut = p3;
	}

}



/** Get the barycentric coordinates of point in a triangle **/
D3DXVECTOR3 barycentric(const D3DXVECTOR3 &point,
						const Triangle &triangle)
{
	// Extract the vertices so you can read this code more easily
	D3DXVECTOR3 v0 = triangle._v0._position,
		        v1 = triangle._v1._position,
	            v2 = triangle._v2._position;

	D3DXVECTOR3 pMinusB,pMinusC;
	D3DXVec3Subtract(&pMinusB,&point,&v1);
	D3DXVec3Subtract(&pMinusC,&point,&v2);

	D3DXVECTOR3 nSubA, nSubB;
	D3DXVec3Cross(&nSubA,&triangle._edge1,&pMinusB);
	D3DXVec3Cross(&nSubB,&triangle._edge2,&pMinusC);

	D3DXVECTOR3 baryCoordinates;

	baryCoordinates.x = D3DXVec3Dot(&triangle._modNormal,&nSubA);
	baryCoordinates.y = D3DXVec3Dot(&triangle._modNormal,&nSubB);
	baryCoordinates.z = 1.0f - baryCoordinates.x - baryCoordinates.y;

	return baryCoordinates;
}

/** Component-wise division **/
D3DXVECTOR3 componentDivision(const D3DXVECTOR3 &dividend,
							  const D3DXVECTOR3 &divisor)
{
	return D3DXVECTOR3(dividend.x/divisor.x,dividend.y/divisor.y,dividend.z/divisor.z);
}

/** Component-wise multiplication **/
D3DXVECTOR3 componentMultiplication(const D3DXVECTOR3 &vector1,
									const D3DXVECTOR3 &vector2)
{
	return D3DXVECTOR3(vector1.x*vector2.x,vector1.y*vector2.y,vector1.z*vector2.z);
}

KDTree::KDTree()
	: _root(0)
{
}

KDTree::~KDTree()
{
	clear();
}

void KDTree::addObject(const RenderablePtr &renderable)
{
	_pendingObjects.insert(renderable);
}

void KDTree::clear()
{
	delete _root;
	_root = 0;
	_pendingObjects.clear();
	_pendingTriangles.clear();
}

void KDTree::getIndexVector(ID3DXMesh *mesh, std::vector<DWORD> &indices)
{
	// Extract data about the index buffer.
	IDirect3DIndexBuffer9 *indexBuffer = 0;
	mesh->GetIndexBuffer(&indexBuffer);
	D3DINDEXBUFFER_DESC indexDesc;
	indexBuffer->GetDesc(&indexDesc);

	// Only allow 16-bit indices.
	D3DFORMAT indexFormat = indexDesc.Format;
	MYASSERT(indexFormat == D3DFMT_INDEX16);
	DWORD numIndices      = indexDesc.Size / sizeof(WORD);

	WORD *indexPtr  = 0;
	mesh->LockIndexBuffer(0, reinterpret_cast<void **>(&indexPtr));
	indices.resize(numIndices);
	for (unsigned i = 0; i < numIndices; i++)
		indices[i] = indexPtr[i];

	mesh->UnlockIndexBuffer();
}

void KDTree::getTriangleVector(
	ID3DXMesh *mesh, 
	const IndexList &indices, 
	TriangleList &triangles)
{
	// Extract the mesh's data.  (Number of faces, vertex stride,
	// number of vertices, etc.)
	DWORD numBytesPerVertex = mesh->GetNumBytesPerVertex();
	DWORD numTriangles      = mesh->GetNumFaces();
	DWORD numVertices       = mesh->GetNumVertices();

	// We require that there be at least 12 bytes per vertex.
	// We need three floats at 4 bytes each to represent the position.
	ASSERTMSG(numBytesPerVertex >= 12, "Vertex stride is too small.");

	// Extract the vertex positions from the indices.
	char *vertexPtr;
	mesh->LockVertexBuffer(0, reinterpret_cast<void **>(&vertexPtr));
	triangles.resize(numTriangles);
	for (unsigned i = 0; i < numTriangles; i++)
	{
		unsigned index1 = (3 * i);
		unsigned index2 = (3 * i) + 1;
		unsigned index3 = (3 * i) + 2;

		DWORD vertexNumber1 = indices[index1];
		DWORD vertexNumber2 = indices[index2];
		DWORD vertexNumber3 = indices[index3];

		float *vertexStart1 = reinterpret_cast<float *>(vertexPtr + vertexNumber1 * numBytesPerVertex);
		float *vertexStart2 = reinterpret_cast<float *>(vertexPtr + vertexNumber2 * numBytesPerVertex);
		float *vertexStart3 = reinterpret_cast<float *>(vertexPtr + vertexNumber3 * numBytesPerVertex);

		CollisionVertex v0(vertexStart1[0], vertexStart1[1], vertexStart1[2]);
		CollisionVertex v1(vertexStart2[0], vertexStart2[1], vertexStart2[2]);
		CollisionVertex v2(vertexStart3[0], vertexStart3[1], vertexStart3[2]);

		triangles[i] = Triangle(v0, v1, v2);
	}
	
	mesh->UnlockVertexBuffer();
}

void KDTree::addPendingTriangles(const TriangleList &trianglesToAdd)
{
	for (TriangleList::const_iterator it = trianglesToAdd.begin();
		 it != trianglesToAdd.end(); it++)
	{
		_pendingTriangles.push_back(*it);
	}
}

void KDTree::build()
{
	_pendingTriangles.clear();
	delete _root;

	for (RenderableSet::const_iterator it = _pendingObjects.begin();
		 it != _pendingObjects.end(); it++)
	{
		const RenderablePtr &currRenderable = *it;
		
		// Only add to the tree if it's static geometry.
		if (!currRenderable->isStatic())
			continue;

		ID3DXMesh *currMesh = currRenderable->getMeshData();

		std::vector<DWORD> indices;
		getIndexVector(currMesh, indices);

		// Extract the list of triangles used by this mesh.
		TriangleList triangleList;
		getTriangleVector(currMesh, indices, triangleList);

		// Add those triangles to the list of triangles of the entire scene.
		addPendingTriangles(triangleList);
	}

	_root = new KDNode(_pendingTriangles);
	subdivide(_root, x_axis, 0);
}


void KDTree::subdivide(
	KDNode *currNode, 
	kd_axis currAxis, 
	int currHeight)
{
	MYASSERT(currNode != 0);

	// Stop splitting if the current node has few enough objects, or if the
	// tree is too tall.
	if (!shouldSubdivide(currNode, currHeight))
	{
		currNode->_plane._axis      = no_axis;
		currNode->_plane._axisValue = 0.0f;
		return;
	}

	bool didSplit = false;
	for (int i = 0; i < 3; i++)
	{
		// Record the splitting axis for this node.
		const BoundingBox &currBox = currNode->getBoundingBox();
		currNode->_plane.setFromBox(currAxis, currBox);

		// Create the left and right subtrees, and place objects into them
		// depending on where the objects in this node are WRT the split plane.
		if (splitNode(currNode))
		{
			didSplit = true;
			break;
		}
		else // Couldn't split along this axis; try another.
			currAxis = getNextAxis(currAxis);
	}

	if (didSplit)
	{
		kd_axis nextAxis = getNextAxis(currAxis);
		subdivide(currNode->_left, nextAxis, currHeight + 1);
		subdivide(currNode->_right, nextAxis, currHeight + 1);
	}
}

bool KDTree::splitNode(KDNode *currNode)
{
	KDNode *left  = new KDNode();
	KDNode *right = new KDNode();
	bool canSplit = false;

	// For each triangle T in currNode, intersect T with currNode's splitting
	// plane, and put T in the appropriate subtree(s).
	for (unsigned i = 0; i < currNode->_triangles.size(); i++)
	{
		const Triangle &currTriangle = currNode->_triangles[i];
		KDPlane::tri_plane_result intersectResult = 
			currNode->_plane.intersectTrianglePlane(currTriangle);

		switch (intersectResult)
		{
		case KDPlane::tri_plane_negative:
			canSplit = true;
			left->addTriangle(currTriangle);
			break;

		case KDPlane::tri_plane_positive:
			canSplit = true;
			right->addTriangle(currTriangle);
			break;

		case KDPlane::tri_plane_intersect:
			left->addTriangle(currTriangle);
			right->addTriangle(currTriangle);
			break;

		default:
			ERRORMSG("Invalid triangle-plane intersection result.");
			break;
		};
	}

	if (canSplit)
	{
		currNode->_left  = left;
		currNode->_right = right;
		currNode->_triangles.clear();
	}
	else
	{
		// Can't split this node, so don't bother keeping the left and right
		// subtrees around anymore.
		delete left;
		delete right;
	}

	return canSplit;
}

bool KDTree::doesRayIntersect(const Ray &ray) const
{
	const D3DXVECTOR3 &origin = ray.origin;
	D3DXVECTOR3 destination   = origin + ray.direction;

	BoundingBox box;
	box.min.x = std::min(origin.x, destination.x);
	box.min.y = std::min(origin.y, destination.y);
	box.min.z = std::min(origin.z, destination.z);

	box.max.x = std::max(origin.x, destination.x);
	box.max.y = std::max(origin.y, destination.y);
	box.max.z = std::max(origin.z, destination.z);
	
	return _root->doesRayIntersect(ray, box);
}

void KDTree::collideWithEntity(EntityPtr entity)
{
	if (entity->getRadius() > 0)
		collideWithEntity(entity,0);
	else
	{
		// If the entity's radius is 0, then it's a bullet.
		// In that case, we treat it as a point (actually, a ray)
		// to get a hopefully faster collision test.
		const D3DXVECTOR3 &origin    = entity->getPos();
		const D3DXVECTOR3 &direction = entity->getVelocity();
		Ray ray(origin, direction);
		
		if (doesRayIntersect(ray))
			entity->takeDamage(0);
	}
}

void KDTree::collideWithEntity(EntityPtr entity, int depth)
{
	// Find the entity's velocity, direction, and magnitude
	D3DXVECTOR3 velocity(entity->getVelocity());
	float magnitude = D3DXVec3Length(&velocity);
	D3DXVECTOR3 direction;
	D3DXVec3Normalize(&direction,&velocity);

	// Exit upon small magnitude
	if(magnitude < util::EPSILON || depth > maxSlideRecursion) {
		entity->setVelocity(D3DXVECTOR3(0,0,0));
		return;
	}

	// Compute the entity's position and radius vector
	float radius = entity->getRadius();
	D3DXVECTOR3 position = entity->getPos();
	position.y += radius; // Offset due to the origin being at the entity's feet

	// Compute a bounding volume to use in the collision test
	//const D3DXVECTOR3 &epsilonVector = D3DXVECTOR3(util::EPSILON,util::EPSILON,util::EPSILON);
	const D3DXVECTOR3 &radiusVector = D3DXVECTOR3(radius,radius,radius);
	D3DXVECTOR3 maxima = position + radiusVector; //+ epsilonVector;
	D3DXVECTOR3 minima = position - radiusVector;// - epsilonVector;
	if(velocity.x > 0) maxima.x += velocity.x; else minima.x += velocity.x;
	if(velocity.y > 0) maxima.y += velocity.y; else minima.y += velocity.y;
	if(velocity.z > 0) maxima.z += velocity.z; else minima.z += velocity.z;

	// At what point does the collision happen, and at what distance from the
	//   ellipsoid approximation?
	bool collisionHappened = false;
	float collisionDistance;
	D3DXVECTOR3 collisionPoint;

	// Get the nearest collision information
	_root->getNearestCollision(collisionHappened, collisionDistance, collisionPoint,
							   position, radius, 1.0f/radius, magnitude, direction, minima,
							   maxima);

	// Handle the collision if it happened
	if(collisionHappened)
	{
		// Scoot the player up to (or back from) the collision
		D3DXVECTOR3 delta(direction);
		delta *= (collisionDistance-0.3);
		position += delta;

		// Define the sliding plane and the pushed-forward offset,
		//  which is in essence intersection point's desired direction
		//  should it not be obstructed
		D3DXVECTOR3 slideNormal;
		D3DXVec3Subtract(&slideNormal,&position,&collisionPoint);
		D3DXVec3Normalize(&slideNormal,&slideNormal);
		D3DXVECTOR3 pushForward(direction);
		pushForward *= (magnitude - collisionDistance);
		
		// Now project the push-forward onto the sliding plane, but augment
		//  via multiplying the coefficient of restitution.
		float pushDotSlide = (1.0f+entity->getCoeffRestitution())*D3DXVec3Dot(&slideNormal,&pushForward);
		D3DXVECTOR3 standardNewVelocity = pushForward - pushDotSlide*slideNormal;
		standardNewVelocity *= (1.0-coeffFriction);
	
		// Now give the entity its new information
		position.y -= radiusVector.y;
		entity->setPos(position);
		entity->setVelocity(standardNewVelocity);

		if(slideNormal.y > 0.7) entity->setOnTheFloor(true);
		entity->takeDamage(0);

		collideWithEntity(entity,depth+1);

	}

}

bool KDTree::shouldSubdivide(KDNode *currNode, int currHeight)
{
	return currNode->_triangles.size() > maxSurfacesPerNode && 
	       currHeight < maxHeight;
}

KDTree::kd_axis KDTree::getNextAxis(kd_axis currAxis) const
{
	switch (currAxis)
	{
	case x_axis:
		return y_axis;
	case y_axis:
		return z_axis;
	case z_axis:
		return x_axis;
	default:
		ERRORMSG("Invalid axis.");
	};

	return no_axis;
}

//////////////////////////////////////////////////////////////////////////
// KD-Tree Splitting Plane hee-haw
//////////////////////////////////////////////////////////////////////////

KDTree::KDPlane::KDPlane(
	float axisValue /* = 0::0f */, 
	kd_axis axis /* = no_axis */)
	: _axisValue(axisValue), _axis(axis)
{
}

KDTree::KDPlane::~KDPlane()
{
}

void KDTree::KDPlane::setFromBox(kd_axis axis, const BoundingBox &box)
{
	switch (axis)
	{
	case x_axis:
		_axisValue = (box.min.x + box.max.x) / 2;
		break;
	case y_axis:
		_axisValue = (box.min.y + box.max.y) / 2;
		break;
	case z_axis:
		_axisValue = (box.min.z + box.max.z) / 2;
		break;
	default:
		ERRORMSG("Invalid axis type.");
	};
	_axis = axis;
}

KDTree::KDPlane::tri_plane_result KDTree::KDPlane::intersectTrianglePlane(
	const Triangle &triangle)
{
	float minAxisVal;
	float maxAxisVal;

	// Gather the triangle's min and max values.
	float minTriX, minTriY, minTriZ;
	float maxTriX, maxTriY, maxTriZ;
	triangle.getMinMax(
		minTriX, minTriY, minTriZ,
		maxTriX, maxTriY, maxTriZ);
	switch (_axis)
	{
	case x_axis:
		minAxisVal = minTriX;
		maxAxisVal = maxTriX;
		break;

	case y_axis:
		minAxisVal = minTriY;
		maxAxisVal = maxTriY;
		break;

	case z_axis:
		minAxisVal = minTriZ;
		maxAxisVal = maxTriZ;
		break;

	default:
		ERRORMSG("Invalid axis type.");
		return tri_plane_error;
	};

	if (_axisValue >= minAxisVal && _axisValue <= maxAxisVal)
		return tri_plane_intersect;
	else if (_axisValue > maxAxisVal)
		return tri_plane_negative;
	else if (_axisValue < minAxisVal)
		return tri_plane_positive;
	else
	{
		ERRORMSG("Unhandled triangle-plane intersection case.");
		return tri_plane_error;
	}
}

//////////////////////////////////////////////////////////////////////////
// KD-Tree Node stuff
//////////////////////////////////////////////////////////////////////////

KDTree::KDNode::KDNode(const TriangleList &triangles)
	: _triangles(triangles), _left(0), _right(0)
{
}

KDTree::KDNode::~KDNode()
{
	delete _left;
	delete _right;
}

bool KDTree::KDNode::isLeaf() const
{
	return _plane._axis == no_axis || (_left == 0 && _right == 0);
}

void KDTree::KDNode::addTriangle(const Triangle &triangle)
{
	_triangles.push_back(triangle);
}

bool KDTree::KDNode::doesRayIntersect(
	const Ray &ray, 
	const BoundingBox &rayBoundingBox) const
{
	if (isLeaf())
	{
		for (unsigned i = 0; i < _triangles.size(); i++)
		{
			if (rayTriangleIntersect(ray, _triangles[i]))
				return true;
		}
		return false;
	}
	else
	{
		const D3DXVECTOR3 &min = rayBoundingBox.min;
		const D3DXVECTOR3 &max = rayBoundingBox.max;
		float minVal;
		float maxVal;
		switch (_plane._axis)
		{
		case x_axis:
			minVal = min.x;
			maxVal = max.x;
			break;

		case y_axis:
			minVal = min.y;
			maxVal = max.y;
			break;

		case z_axis:
			minVal = min.z;
			maxVal = max.z;
			break;

		default:
			ERRORMSG("Invalid axis!");
			break;
		};

		if (minVal <= _plane._axisValue &&
		    maxVal >= _plane._axisValue)
		{
			return _left->doesRayIntersect(ray, rayBoundingBox) || 
				   _right->doesRayIntersect(ray, rayBoundingBox);
		}
		else if (maxVal < _plane._axisValue)
			return _left->doesRayIntersect(ray, rayBoundingBox);
		else
			return _right->doesRayIntersect(ray, rayBoundingBox);
	}
}

BoundingBox KDTree::KDNode::getBoundingBox() const
{
	ASSERTMSG(!_triangles.empty(), "KDNode has no triangles!");

	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	minX = minY = minZ =  std::numeric_limits<float>::max();
	maxX = maxY = maxZ = -std::numeric_limits<float>::max();

	for (TriangleList::const_iterator it = _triangles.begin();
		 it != _triangles.end(); it++)
	{
		const Triangle &currTriangle = *it;

		float minTriX, minTriY, minTriZ;
		float maxTriX, maxTriY, maxTriZ;
		currTriangle.getMinMax(
			minTriX, minTriY, minTriZ,
			maxTriX, maxTriY, maxTriZ);

		// Update the running min and max values.
		if (minTriX < minX)	
			minX = minTriX;
		if (minTriY < minY) 
			minY = minTriY;
		if (minTriZ < minZ) 
			minZ = minTriZ;

		if (maxTriX > maxX) 
			maxX = maxTriX;
		if (maxTriY > maxY) 
			maxY = maxTriY;
		if (maxTriZ > maxZ) 
			maxZ = maxTriZ;
	}

	BoundingBox box;
	box.min = D3DXVECTOR3(minX, minY, minZ);
	box.max = D3DXVECTOR3(maxX, maxY, maxZ);

	return box;
}

void KDTree::KDNode::getNearestCollision(bool &collisionHappened, float &collisionDistance, D3DXVECTOR3 &collisionPoint,
										 const D3DXVECTOR3 &position, float radius, float inverseRadius, float magnitude,
										 const D3DXVECTOR3 &direction, const D3DXVECTOR3 &minima, const D3DXVECTOR3 &maxima)
{
	if (isLeaf())
	{

		Ray intersectionRay;
		D3DXVECTOR3 intersectionPoint;
		D3DXVECTOR3 baryCoords;
		D3DXVECTOR3 normal;
		bool detectedIntersection;
		float intersectionDistance;

		for (int it = 0; it != _triangles.size(); it++)
		{
			// Check for collisions only for triangles we're running into.
			const Triangle &currTriangle = _triangles[it];
			normal = currTriangle._normal;
			if(D3DXVec3Dot(&normal,&direction) <= 0)
			{
				// The collision point on the ellipsoid is where the ellipsoid's normal
				//   vector is pointing in the direction of the plane's negative normal
				// Using the ellipsoid equation x^2/a^2 + y^2/b^2 + z^2/c^2 = 1, we assume
				//   there is some k for which we can multiply the negative plane normal by
				//   to get the point on the ellipse, and find it to be sqrt(1/alpha), where
				//   alpha is the x^2/a^2 + y^2/b^2 + z^2/c^2 for plane normal <x,y,z>

				// Construct a ray at the ellipsoid collision point with the direction of velocity
				//   and see where we bump into the triangle's plane.
				intersectionRay.direction = direction;
				intersectionRay.origin = position-radius*normal;
				intersectionDistance;
				rayPlaneIntersect(intersectionDistance,intersectionRay,
								  currTriangle._v0._position,normal);

				// No intersection is possible
				if(intersectionDistance > magnitude) continue;

				// Find the point of intersection, and make sure it's inside the triangle.  To do this,
				//   assume we have detected an intersection, and correct this assumption if it fails in
				//   the corner/edge case.
				intersectionPoint = intersectionRay.origin + intersectionDistance*intersectionRay.direction;
				baryCoords = barycentric(intersectionPoint,currTriangle);
				detectedIntersection = true;
				if(baryCoords.x < 0 || baryCoords.y < 0 || baryCoords.z < 0)
				{
					closestOnBoundary(&intersectionPoint, position,currTriangle);
					detectedIntersection = raySphereIntersect(intersectionDistance,
															   intersectionPoint,
															   -direction,
															   position, inverseRadius);
				}

				if(detectedIntersection && intersectionDistance < magnitude && intersectionDistance > -15 &&
				   (!collisionHappened || intersectionDistance < collisionDistance)
				     )
				{
					collisionHappened = true;
					collisionDistance = intersectionDistance;
					collisionPoint = intersectionPoint;
				}
				
			}
		}
	}
	else
	{
		switch (_plane._axis)
		{
		case x_axis:

			if (minima.x <= _plane._axisValue &&
				maxima.x >= _plane._axisValue)
			{
				_left->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										   position,radius,inverseRadius,magnitude,direction,minima,maxima);
				_right->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										   position,radius,inverseRadius,magnitude,direction,minima,maxima);
			}
			else if (maxima.x < _plane._axisValue)
				_left->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										   position,radius,inverseRadius,magnitude,direction,minima,maxima);
			else
				_right->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										    position,radius,inverseRadius,magnitude,direction,minima,maxima);
			break;

		case y_axis:

			if (minima.y <= _plane._axisValue &&
				maxima.y >= _plane._axisValue)
			{
				_left->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										   position,radius,inverseRadius,magnitude,direction,minima,maxima);
				_right->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										    position,radius,inverseRadius,magnitude,direction,minima,maxima);
			}
			else if (maxima.y < _plane._axisValue)
				_left->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										   position,radius,inverseRadius,magnitude,direction,minima,maxima);
			else
				_right->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										    position,radius,inverseRadius,magnitude,direction,minima,maxima);
			break;

		case z_axis:

			if (minima.z <= _plane._axisValue &&
				maxima.z >= _plane._axisValue)
			{
				_left->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										   position,radius,inverseRadius,magnitude,direction,minima,maxima);
				_right->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										    position,radius,inverseRadius,magnitude,direction,minima,maxima);
			}
			else if (maxima.z < _plane._axisValue)
				_left->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										   position,radius,inverseRadius,magnitude,direction,minima,maxima);
			else
				_right->getNearestCollision(collisionHappened,collisionDistance,collisionPoint,
										    position,radius,inverseRadius,magnitude,direction,minima,maxima);
			break;

		case no_axis:
			ERRORMSG("Invalid axis!");
			break;
		};
	}
}
