#ifndef RAY_HPP
#define RAY_HPP

//
// Simple ray, used to test for collisions against the kd-tree, for line of
// sight testing.
//

#include <d3dx9.h>

struct Ray
{
	Ray()
	{
	}
	Ray(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &direction)
		: origin(origin), direction(direction)
	{
	}

	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;
};

#endif