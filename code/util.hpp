#ifndef UTIL_HPP
#define UTIL_HPP

//
// Includes miscellaneous utility functions.
//

#include <string>
#include <sstream>
#include <limits>
#include <cctype>
#include <d3dx9.h>

#include "common.hpp"
#include "timer.hpp"

namespace util
{
	// Some handy constants.
	const float INFINITY = FLT_MAX;
	const float EPSILON  = 0.000001f;

	template<typename T>
	std::string toString(T toConvert)
	{
		std::stringstream s;
		s<<toConvert;
		return s.str();
	}

	inline float randFloat(float min, float max)
	{
		return (float)(((double)rand() / (double)RAND_MAX) * (max-min) + min);
	}

	inline DWORD floatToDWORD(float f)
	{
		return *(reinterpret_cast<DWORD *>(&f));
	}

	template<typename T> 
	inline
	void release(T &t)
	{
		if( t )
		{
			t->Release();
			t = 0;
		}
	}

	// Returns the first argument clamped between minVal and maxVal.
	template<typename T> 
	inline
	T clamp(T toClamp, T minVal, T maxVal)
	{
		if (toClamp < minVal)       return minVal;
		else if (toClamp > maxVal)  return maxVal;
		else                        return toClamp;
	}

	template<typename T>
	inline
	T wrap(T toWrap, T minVal, T maxVal)
	{
		if (toWrap < minVal)      return maxVal;
		else if (toWrap > maxVal) return minVal;
		else                      return toWrap;
	}

	inline
	float wrapAngle(float toWrap, float minVal, float maxVal)
	{
		float spacing = maxVal - minVal;
		while(toWrap < minVal) toWrap += spacing;//2*D3DX_PI;
		while(toWrap > maxVal) toWrap -= spacing;//2*D3DX_PI;
		return toWrap;
	}

	// Linear interpolation.
	inline float lerp(float a, float b, float t)
	{
		return a - (a * t) + (b * t);
	}

	template<typename T>
	inline
	bool rectangleContainsPoint(
		T x, 
		T y,
		T topLeftX, 
		T topLeftY, 
		T width, 
		T height)
	{
		return x >= topLeftX && x <= topLeftX + width && 
			   y >= topLeftY && y <= topLeftY + height;
	}

	// Case insensitive string comparison functor.
	class CaseInsensitiveLess
	{
	public:
		CaseInsensitiveLess()  {}
		~CaseInsensitiveLess() {}

		bool operator() (
			const std::string &left, 
			const std::string &right) const
		{
			return (strcasecmp(left.c_str(), right.c_str()) < 0);
		}
	};

	template<typename T>
	bool toleranceCompare(T t1, T t2, T tolerance)
	{
		T diff = t2 - t1;
		return (diff < tolerance && diff > -tolerance);
	}

	inline bool Vector3Compare(D3DXVECTOR3 v1, D3DXVECTOR3 v2, float tolerance = EPSILON)
	{
		D3DXVECTOR3 diff = v2 - v1;
		
		return (toleranceCompare(v1.x,v2.x,tolerance) && 
			    toleranceCompare(v1.y,v2.y,tolerance) && 
				toleranceCompare(v1.z,v2.z,tolerance));
	}

	// Verlet integration functions for updating position and velocity.
	inline void updatePositionAndVelocity(
		D3DXVECTOR3 &position,
		const D3DXVECTOR3 &oldPosition, 
		D3DXVECTOR3 &velocity, 
		const D3DXVECTOR3 &acceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	{
		float dt = getDtSeconds();
		position = (position * 2) - oldPosition + (acceleration * dt * dt);

		velocity = (position - oldPosition) / (2 * dt);
	}
};

#endif