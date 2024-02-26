#pragma once

#include "Math/Geometry/TDisk.h"
#include "Math/TOrthonormalBasis3.h"

namespace ph::math
{

template<typename T>
inline TDisk<T> TDisk<T>::makeUnit()
{
	return TDisk(1);
}

template<typename T>
inline TDisk<T>::TDisk(const T radius) : 
	m_radius(radius)
{
	PH_ASSERT_GT(radius, static_cast<T>(0));
}

template<typename T>
inline T TDisk<T>::getArea() const
{
	return constant::pi<T> * m_radius * m_radius;
}

template<typename T>
inline TVector3<T> TDisk<T>::sampleToSurface(const std::array<T, 2>& sample) const
{
	const auto surfacePos2D = sampleToSurface2D(sample);
	return TVector3<T>(surfacePos2D.y, static_cast<T>(0), surfacePos2D.x);
}

template<typename T>
inline TVector3<T> TDisk<T>::sampleToSurface(const std::array<T, 2>& sample, T* const out_pdfA) const
{
	const auto surfacePos2D = sampleToSurface2D(sample, out_pdfA);
	return TVector3<T>(surfacePos2D.y, static_cast<T>(0), surfacePos2D.x);
}

template<typename T>
inline TVector2<T> TDisk<T>::sampleToSurface2D(const std::array<T, 2>& sample) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], static_cast<T>(0), static_cast<T>(1));
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], static_cast<T>(0), static_cast<T>(1));
	PH_ASSERT_GE(m_radius, static_cast<T>(0));

	const T r   = std::sqrt(sample[0]);
	const T phi = constant::two_pi<real> * sample[1];

	const auto localUnitPos = TVector2<T>(
		r * std::cos(phi),
		r * std::sin(phi));

	return localUnitPos.mul(m_radius);
}

template<typename T>
inline TVector2<T> TDisk<T>::sampleToSurface2D(const std::array<T, 2>& sample, T* const out_pdfA) const
{
	PH_ASSERT(out_pdfA);
	*out_pdfA = uniformSamplePdfA();

	return sampleToSurface2D(sample);
}

template<typename T>
inline TVector3<T> TDisk<T>::sampleToSurfaceOriented(
	const std::array<T, 2>& sample,
	const TVector3<T>&      normal,
	const TVector3<T>&      offset) const
{
	const auto& diskPos2d = sampleToSurface2D(sample);
	const auto& basis     = TOrthonormalBasis3<T>::makeFromUnitY(normal);

	return (basis.getZAxis() * diskPos2d.x) + 
	       (basis.getXAxis() * diskPos2d.y) + 
	       offset;
}

template<typename T>
inline TVector3<T> TDisk<T>::sampleToSurfaceOriented(
	const std::array<T, 2>& sample,
	T* const                out_pdfA,
	const TVector3<T>&      normal,
	const TVector3<T>&      offset) const
{
	PH_ASSERT(out_pdfA);
	*out_pdfA = uniformSamplePdfA();

	return sampleToSurfaceOriented(sample, normal, offset);
}

template<typename T>
inline T TDisk<T>::uniformSamplePdfA() const
{
	// PDF_A is 1/(pi*r^2)
	return static_cast<T>(1) / getArea();
}

}// end namespace ph::math
