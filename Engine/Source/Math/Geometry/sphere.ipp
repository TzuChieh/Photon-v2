#pragma once

#include "Math/Geometry/sphere.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

namespace ph
{

namespace math
{

inline bool is_intersecting_sphere(
	const Ray&      ray,
	const Vector3R& center,
	const real      radius,
	real* const     out_hitT)
{
	PH_ASSERT_GE(radius, 0);
	PH_ASSERT(out_hitT);

	// ray origin:         o
	// ray direction:      d
	// sphere center:      c
	// sphere radius:      r
	// intersection point: p
	// vector dot:         *
	// ray equation:       o + td (t is a scalar variable)
	//
	// To find the intersection point, the length of vector (td - oc) must equals r.
	// This is equivalent to (td - oc)*(td - oc) = r^2. After reformatting, we have
	//
	//              t^2(d*d) - 2t(d*oc) + (oc*oc) - r^2 = 0     --- (1)
	//
	// Solving equation (1) for t will yield the intersection point (o + td).
	
	const Vector3D rayO(ray.getOrigin());
	const Vector3D rayD(ray.getDirection());

	// vector from ray origin (o) to sphere center (c)
	//
	const Vector3D oc = Vector3D(center).sub(rayO);
	
	const float64 a = rayD.dot(rayD);// a in equation (1)
	const float64 b = rayD.dot(oc);// b in equation (1) (-2 is cancelled while solving t)
	const float64 c = oc.dot(oc) - static_cast<float64>(radius) * radius;// c in equation (1)

	float64 D = b * b - a * c;
	if(D < 0.0)
	{
		return false;
	}
	else
	{
		D = std::sqrt(D);

		const float64 reciA = 1.0 / a;

		// pick the closest point in front of ray tail
		// t = (b +- D) / a
		//
		const float64 t1 = (b - D) * reciA;
		const float64 t2 = (b + D) * reciA;

		PH_ASSERT_MSG(t1 <= t2, "\n"
			"t1            = " + std::to_string(t1) + "\n"
			"t2            = " + std::to_string(t2) + "\n"
			"(a, b, c)     = (" + std::to_string(a) + ", " + std::to_string(b) + ", " + std::to_string(c) + ")\n"
			"ray-origin    = " + rayO.toString() + "\n"
			"ray-direction = " + rayD.toString());

		// t1 is smaller than t2, we test t1 first
		//
		float64 t;
		if(ray.getMinT() < t1 && t1 < ray.getMaxT())
		{
			t = t1;
		}
		else if(ray.getMinT() < t2 && t2 < ray.getMaxT())
		{
			t = t2;
		}
		else
		{
			return false;
		}

		*out_hitT = static_cast<real>(t);
		return true;
	}
}

template<typename T>
inline T sphere_area(const T radius)
{
	PH_ASSERT_GE(radius, T(0));

	return constant::four_pi<T> * radius * radius;
}

}// end namespace math

}// end namespace ph
