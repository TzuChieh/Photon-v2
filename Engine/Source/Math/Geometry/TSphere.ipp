#pragma once

#include "Math/Geometry/TSphere.h"
#include "Common/assertion.h"
#include "Math/constant.h"

#include <cmath>

namespace ph::math
{

template<typename T>
inline TSphere<T> TSphere<T>::makeUnit()
{
	return TSphere(1);
}

template<typename T>
inline TSphere<T>::TSphere(const T radius) : 
	m_radius(radius)
{
	PH_ASSERT_GE(radius, T(0));
}

template<typename T>
inline bool TSphere<T>::isIntersecting(
	const TLineSegment<T>& segment,
	real* const            out_hitT) const
{
	PH_ASSERT_GE(m_radius, T(0));
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
	
	// FIXME: T may be more precise than float64

	const Vector3D segmentO(segment.getOrigin());
	const Vector3D segmentD(segment.getDirection());

	// Vector from ray origin (o) to sphere center (c)
	const Vector3D oc = Vector3D(0).sub(segmentO);
	
	// a in equation (1)
	const float64 a = segmentD.dot(segmentD);

	// b in equation (1) (-2 is cancelled while solving t)
	const float64 b = segmentD.dot(oc);

	// c in equation (1)
	const float64 c = oc.dot(oc) - static_cast<float64>(m_radius) * m_radius;

	float64 D = b * b - a * c;
	if(D < 0.0)
	{
		return false;
	}
	else
	{
		D = std::sqrt(D);

		const float64 rcpA = 1.0 / a;

		// t = (b +- D) / a
		// Pick the closest point in front of ray tail
		const float64 t1 = (b - D) * rcpA;
		const float64 t2 = (b + D) * rcpA;

		PH_ASSERT_MSG(t1 <= t2, "\n"
			"t1            = " + std::to_string(t1) + "\n"
			"t2            = " + std::to_string(t2) + "\n"
			"(a, b, c)     = (" + std::to_string(a) + ", " + std::to_string(b) + ", " + std::to_string(c) + ")\n"
			"ray-origin    = " + segmentO.toString() + "\n"
			"ray-direction = " + segmentD.toString());

		// t1 is smaller than t2, we test t1 first
		//
		float64 t;
		if(segment.getMinT() < t1 && t1 < segment.getMaxT())
		{
			t = t1;
		}
		else if(segment.getMinT() < t2 && t2 < segment.getMaxT())
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
inline T TSphere<T>::getArea() const
{
	PH_ASSERT_GE(m_radius, T(0));

	return constant::four_pi<T> * m_radius * m_radius;
}

template<typename T>
inline TVector3<T> TSphere<T>::sampleToSurfaceArchimedes(const std::array<T, 2>& sample) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], static_cast<T>(0), static_cast<T>(1));
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], static_cast<T>(0), static_cast<T>(1));
	PH_ASSERT_GE(m_radius, static_cast<T>(0));

	const T y   = static_cast<T>(2) * (sample[0] - static_cast<T>(0.5));
	const T phi = constant::two_pi<T> * sample[1];
	const T r   = std::sqrt(std::max(static_cast<T>(1) - y * y, static_cast<T>(0)));

	const auto localUnitPos = TVector3<T>(
		r * std::sin(phi),
		y,
		r * std::cos(phi));

	return localUnitPos * m_radius;
}

template<typename T>
inline TVector3<T> TSphere<T>::sampleToSurfaceArchimedes(
	const std::array<T, 2>& sample, T* const out_pdfA) const
{
	PH_ASSERT(out_pdfA);

	// PDF_A is 1/(4*pi*r^2)
	*out_pdfA = static_cast<T>(1) / getArea();

	return sampleToSurfaceArchimedes(sample);
}

template<typename T>
inline TVector3<T> TSphere<T>::phiThetaToSurface(const T phi, const T theta) const
{
	const T zxPlaneRadius = std::sin(theta);

	const auto localUnitPos = TVector3<T>(
		zxPlaneRadius * std::sin(phi),
		std::cos(theta),
		zxPlaneRadius * std::cos(phi));

	return localUnitPos * m_radius;
}

}// end namespace ph::math
