#pragma once

#include "Math/Geometry/TSphere.h"
#include "Common/assertion.h"
#include "Math/constant.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/TVector2.h"
#include "Math/TMatrix2.h"

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

	*out_pdfA = uniformSurfaceSamplePdfA();

	return sampleToSurfaceArchimedes(sample);
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
