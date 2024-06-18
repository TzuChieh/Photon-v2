#pragma once

#include "Math/Geometry/TSphere.h"
#include "Math/constant.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/TVector2.h"
#include "Math/TMatrix2.h"

#include <Common/assertion.h>

#include <cmath>
#include <algorithm>
#include <type_traits>

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
	//return isIntersectingNaive(segment, out_hitT);
	return isIntersectingHearnBaker(segment, out_hitT);
}

//template<typename T>
//inline bool TSphere<T>::isIntersectingRefined(
//	const TLineSegment<T>& segment,
//	T* const               out_hitT) const
//{
//	const auto segmentOriginToCentroid = math::TVector3<T>(0) - segment.getOrigin();
//	const auto approxHitT = segmentOriginToCentroid.dot(segment.getDirection());
//
//	const TLineSegment<T> shiftedSegment(
//		segment.getPoint(approxHitT),
//		segment.getDirection(),
//		segment.getMinT() - approxHitT,
//		segment.getMaxT() - approxHitT);
//	if(!isIntersecting(shiftedSegment, out_hitT))
//	{
//		return false;
//	}
//
//	*out_hitT += approxHitT;
//	return true;
//}

template<typename T>
inline bool TSphere<T>::isInside(const TVector3<T>& point) const
{
	// Checks whether the point is within radius (sphere center = origin)
	return point.lengthSquared() < math::squared(m_radius);
}

template<typename T>
inline bool TSphere<T>::isIntersectingNaive(
	const TLineSegment<T>& segment,
	real* const out_hitT) const
{
	PH_ASSERT_GE(m_radius, T(0));
	PH_ASSERT(out_hitT);

	// Construct a ray/line then use its parametric form to test against the sphere and solve for t
	//
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

	const Vector3D rayO(segment.getOrigin());
	const Vector3D rayD(segment.getDir());

	// Vector from ray origin (o) to sphere center (c)
	const Vector3D oc = Vector3D(0).sub(rayO);
	
	// a in equation (1)
	const float64 a = rayD.dot(rayD);

	// b in equation (1) (-2 is cancelled while solving t)
	const float64 b = rayD.dot(oc);

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
		// t1 must be <= t2 (rcpA & D always > 0)
		const float64 t1 = (b - D) * rcpA;
		const float64 t2 = (b + D) * rcpA;

		PH_ASSERT_MSG(t1 <= t2, "\n"
			"t1            = " + std::to_string(t1) + "\n"
			"t2            = " + std::to_string(t2) + "\n"
			"(a, b, c)     = (" + std::to_string(a) + ", " + std::to_string(b) + ", " + std::to_string(c) + ")\n"
			"ray-origin    = " + rayO.toString() + "\n"
			"ray-direction = " + rayD.toString());

		// We now know that the ray/line intersects the sphere, but it may not be the case for the line
		// segment. We test all cases with t1 <= t2 being kept in mind:

		float64 tClosest;

		// t1 is smaller than t2, we test t1 first
		if(segment.getMinT() <= t1 && t1 <= segment.getMaxT())
		{
			tClosest = t1;
		}
		// test t2
		else if(segment.getMinT() <= t2 && t2 <= segment.getMaxT())
		{
			tClosest = t2;
		}
		// no intersection found in [t_min, t_max] (NaN aware)
		else
		{
			return false;
		}

		PH_ASSERT_IN_RANGE_INCLUSIVE(tClosest, segment.getMinT(), segment.getMaxT());

		*out_hitT = static_cast<real>(tClosest);
		return true;
	}
}

// References:
// [1] Ray Tracing Gems Chapter 7: Precision Improvements for Ray/Sphere Intersection
// [2] Best reference would be code for chapter 7 with method 5 (our line direction is not normalized)
//     https://github.com/Apress/ray-tracing-gems/
// [3] https://github.com/NVIDIAGameWorks/Falcor (Source/Falcor/Utils/Geometry/IntersectionHelpers.slang)
//
template<typename T>
inline bool TSphere<T>::isIntersectingHearnBaker(
	const TLineSegment<T>& segment,
	real* const out_hitT) const
{
	PH_ASSERT_GE(m_radius, T(0));
	PH_ASSERT(out_hitT);

	// We use the same notation as `isIntersectingNaive()` for the geometries

	const Vector3D rayO(segment.getOrigin());
	const Vector3D rayD(segment.getDir());
	const Vector3D unitRayD = rayD.normalize();

	// Vector from sphere center (c) to ray origin (o)
	// (vector f in Ray Tracing Gems, note this is the negated version of oc in `isIntersectingNaive()`)
	const Vector3D co = rayO;// rayO - Vector3D(0);

	const float64 r2 = static_cast<float64>(m_radius) * m_radius;

	// a in equation (1) in `isIntersectingNaive()`
	// (same as in RT Gems)
	const float64 a = rayD.dot(rayD);

	// b term in RT Gems eq. 8 without the negative sign (handled in q later to save one negation)
	const float64 b = co.dot(rayD);

	// vector fd in RT Gems eq. 5 (curvy_L^2)
	const Vector3D fd = co - unitRayD * co.dot(unitRayD);

	// Basically the discriminant term in RT Gems eq. 5 with 4s and 2s eliminated
	const float64 D = a * (r2 - fd.dot(fd));
	if(D < 0.0)
	{
		return false;
	}
	else
	{
		// c in equation (1) in `isIntersectingNaive()`
		// (same as in RT Gems)
		const float64 c = co.dot(co) - r2;

		const float64 sqrtD = std::sqrt(D);

		// q term in RT Gems eq. 10
		const float64 q = (b >= 0.0) ? -sqrtD - b : sqrtD - b;

		// t0 & t1 term in RT Gems eq. 10 (here we let t0 <= t1)
		// (unlike `isIntersectingNaive()`, we cannot determine which one is smaller due to its formulation)
		float64 t0 = c / q;
		float64 t1 = q / a;
		if(t0 > t1)
		{
			std::swap(t0, t1);
		}

		PH_ASSERT_MSG(t0 <= t1, "\n"
			"t0            = " + std::to_string(t0) + "\n"
			"t1            = " + std::to_string(t1) + "\n"
			"(a, b, c)     = (" + std::to_string(a) + ", " + std::to_string(b) + ", " + std::to_string(c) + ")\n"
			"ray-origin    = " + rayO.toString() + "\n"
			"ray-direction = " + rayD.toString());

		// We now know that the ray/line intersects the sphere, but it may not be the case for the line
		// segment. We test all cases with t0 <= t1 being kept in mind:
		
		float64 tClosest;

		// t0 is smaller than t1, we test t0 first
		if(segment.getMinT() <= t0 && t0 <= segment.getMaxT())
		{
			tClosest = t0;
		}
		// test t1
		else if(segment.getMinT() <= t1 && t1 <= segment.getMaxT())
		{
			tClosest = t1;
		}
		// no intersection found in [t_min, t_max] (NaN aware)
		else
		{
			return false;
		}

		PH_ASSERT_IN_RANGE_INCLUSIVE(tClosest, segment.getMinT(), segment.getMaxT());

		*out_hitT = static_cast<real>(tClosest);
		return true;
	}
}

template<typename T>
inline T TSphere<T>::getRadius() const
{
	return m_radius;
}

template<typename T>
inline T TSphere<T>::getArea() const
{
	return constant::four_pi<T> * m_radius * m_radius;
}

template<typename T>
inline TAABB3D<T> TSphere<T>::getAABB() const
{
	constexpr auto EXPANSION_RATIO = T(0.0001);

	return TAABB3D<T>(
		TVector3<T>(-m_radius, -m_radius, -m_radius),
		TVector3<T>( m_radius,  m_radius,  m_radius))
		.expand(TVector3<T>(EXPANSION_RATIO * m_radius));
}

template<typename T>
inline bool TSphere<T>::mayOverlapVolume(const TAABB3D<T>& volume) const
{
	// Intersection test for solid box and hollow sphere.
	// Reference: Jim Arvo's algorithm in Graphics Gems 2

	const T radius2 = math::squared(m_radius);

	// These variables are gonna store minimum and maximum squared distances 
	// from the sphere's center to the AABB volume.
	T minDist2 = T(0);
	T maxDist2 = T(0);

	T a, b;

	a = math::squared(volume.getMinVertex().x());
	b = math::squared(volume.getMaxVertex().x());
	maxDist2 += std::max(a, b);
	if     (T(0) < volume.getMinVertex().x()) minDist2 += a;
	else if(T(0) > volume.getMaxVertex().x()) minDist2 += b;

	a = math::squared(volume.getMinVertex().y());
	b = math::squared(volume.getMaxVertex().y());
	maxDist2 += std::max(a, b);
	if     (T(0) < volume.getMinVertex().y()) minDist2 += a;
	else if(T(0) > volume.getMaxVertex().y()) minDist2 += b;

	a = math::squared(volume.getMinVertex().z());
	b = math::squared(volume.getMaxVertex().z());
	maxDist2 += std::max(a, b);
	if     (T(0) < volume.getMinVertex().z()) minDist2 += a;
	else if(T(0) > volume.getMaxVertex().z()) minDist2 += b;

	return minDist2 <= radius2 && radius2 <= maxDist2;
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

	*out_pdfA = uniformSurfaceSamplePdfA();

	return sampleToSurfaceArchimedes(sample);
}

template<typename T>
inline TVector3<T> TSphere<T>::sampleToSurfaceAbsCosThetaWeighted(const std::array<T, 2>& sample) const
{
	PH_ASSERT_LE(static_cast<T>(0), sample[0]); PH_ASSERT_LE(sample[0], static_cast<T>(1));
	PH_ASSERT_LE(static_cast<T>(0), sample[1]); PH_ASSERT_LE(sample[1], static_cast<T>(1));

	const auto ySign         = math::sign(sample[1] - static_cast<T>(0.5));
	const auto reusedSample1 = ySign * static_cast<T>(2) * (sample[1] - static_cast<T>(0.5));

	const T phi     = constant::two_pi<T> * sample[0];
	const T yValue  = ySign * std::sqrt(reusedSample1);
	const T yRadius = std::sqrt(static_cast<T>(1) - yValue * yValue);// TODO: y*y is in fact valueB?

	const auto localUnitPos = TVector3<T>(
		std::sin(phi) * yRadius,
		yValue,
		std::cos(phi) * yRadius);

	return localUnitPos.mul(m_radius);
}

template<typename T>
inline TVector3<T> TSphere<T>::sampleToSurfaceAbsCosThetaWeighted(
	const std::array<T, 2>& sample, T* const out_pdfA) const
{
	const auto localPos = sampleToSurfaceAbsCosThetaWeighted(sample);

	PH_ASSERT_GE(localPos.y(), static_cast<T>(0));
	const T cosTheta = localPos.y() / m_radius;

	// PDF_A is |cos(theta)|/(pi*r^2)
	PH_ASSERT(out_pdfA);
	*out_pdfA = std::abs(cosTheta) / (constant::pi<real> * m_radius * m_radius);

	return localPos;
}

template<typename T>
inline T TSphere<T>::uniformSurfaceSamplePdfA() const
{
	// PDF_A is 1/(4*pi*r^2)
	return static_cast<T>(1) / getArea();
}

template<typename T>
inline TVector2<T> TSphere<T>::surfaceToLatLong01(const TVector3<T>& surface) const
{
	using namespace math::constant;

	const TVector2<T>& phiTheta = surfaceToPhiTheta(surface);

	return {
		phiTheta.x() / two_pi<T>,       // [0, 1]
		(pi<T> - phiTheta.y()) / pi<T>};// [0, 1]
}

template<typename T>
inline TVector2<T> TSphere<T>::latLong01ToPhiTheta(const TVector2<T>& latLong01) const
{
	using namespace math::constant;

	const T phi   = latLong01.x() * two_pi<T>;
	const T theta = (static_cast<T>(1) - latLong01.y()) * pi<T>;

	return {phi, theta};
}

template<typename T>
inline TVector3<T> TSphere<T>::latLong01ToSurface(const TVector2<T>& latLong01) const
{
	return phiThetaToSurface(latLong01ToPhiTheta(latLong01));
}

template<typename T>
inline TVector2<T> TSphere<T>::surfaceToPhiTheta(const TVector3<T>& surface) const
{
	using namespace math::constant;

	PH_ASSERT_GT(m_radius, 0);

	const math::TVector3<T>& unitDir = surface.div(m_radius);

	const T cosTheta = math::clamp(unitDir.y(), static_cast<T>(-1), static_cast<T>(1));

	const T theta  = std::acos(cosTheta);                                      // [  0,   pi]
	const T phiRaw = std::atan2(unitDir.x(), unitDir.z());                     // [-pi,   pi]
	const T phi    = phiRaw >= static_cast<T>(0) ? phiRaw : two_pi<T> + phiRaw;// [  0, 2*pi]

	return {phi, theta};
}

template<typename T>
inline TVector3<T> TSphere<T>::phiThetaToSurface(const TVector2<T>& phiTheta) const
{
	const T phi   = phiTheta.x();
	const T theta = phiTheta.y();

	const T zxPlaneRadius = std::sin(theta);

	const auto localUnitPos = TVector3<T>(
		zxPlaneRadius * std::sin(phi),
		std::cos(theta),
		zxPlaneRadius * std::cos(phi));

	return localUnitPos * m_radius;
}

template<typename T>
template<typename SurfaceToUv>
inline std::pair<TVector3<T>, TVector3<T>> TSphere<T>::surfaceDerivativesWrtUv(
	const TVector3<T>& surface,
	SurfaceToUv        surfaceToUv,
	T                  hInRadians) const
{
	static_assert(std::is_invocable_r_v<TVector2<T>, SurfaceToUv, TVector3<T>>,
		"A surface to UV mapper must accept Vector3 position and return Vector2 "
		"UV or any type that is convertible to the mentioned ones");

	PH_ASSERT_GT(hInRadians, 0);
	
	const math::TVector3<T>& normal = surface.div(m_radius);
	hInRadians = std::min(hInRadians, math::to_radians<T>(45));

	// Calculate displacement vectors on hit normal's tangent plane
	// (with small angle approximation)

	const T delta = m_radius * std::tan(hInRadians);

	const auto& hitBasis = math::TOrthonormalBasis3<T>::makeFromUnitY(normal);
	const math::TVector3<T>& dx = hitBasis.getXAxis().mul(delta);
	const math::TVector3<T>& dz = hitBasis.getZAxis().mul(delta);

	// Compute partial derivatives with 2nd-order approximation

	// Find delta positions on the sphere from displacement vectors
	const math::TVector3<T>& negX = surface.sub(dx).normalize().mul(m_radius);
	const math::TVector3<T>& posX = surface.add(dx).normalize().mul(m_radius);
	const math::TVector3<T>& negZ = surface.sub(dz).normalize().mul(m_radius);
	const math::TVector3<T>& posZ = surface.add(dz).normalize().mul(m_radius);

	// Find delta uvw vectors
	const math::TVector2<T>& negXuv = surfaceToUv(negX);
	const math::TVector2<T>& posXuv = surfaceToUv(posX);
	const math::TVector2<T>& negZuv = surfaceToUv(negZ);
	const math::TVector2<T>& posZuv = surfaceToUv(posZ);

	const math::TMatrix2<T> uvwDiff(
		posXuv.x() - negXuv.x(), posXuv.y() - negXuv.y(),
		posZuv.x() - negZuv.x(), posZuv.y() - negZuv.y());
	const auto xDiff = posX - negX;
	const auto zDiff = posZ - negZ;
	const std::array<std::array<T, 2>, 3> bs = {
		xDiff.x(), zDiff.x(),
		xDiff.y(), zDiff.y(),
		xDiff.z(), zDiff.z() };

	// Calculate positional partial derivatives
	math::TVector3<T> dPdU, dPdV;
	std::array<std::array<T, 2>, 3> xs;
	if(uvwDiff.solve(bs, &xs))
	{
		dPdU.x() = xs[0][0]; dPdV.x() = xs[0][1];
		dPdU.y() = xs[1][0]; dPdV.y() = xs[1][1];
		dPdU.z() = xs[2][0]; dPdV.z() = xs[2][1];
	}
	else
	{
		dPdU = hitBasis.getZAxis();
		dPdV = hitBasis.getXAxis();
	}

	return {dPdU, dPdV};
}

}// end namespace ph::math
