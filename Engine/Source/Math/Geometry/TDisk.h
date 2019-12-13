#pragma once

#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

#include <cmath>
#include <array>

namespace ph::math
{

template<typename T>
class TDisk final
{
public:
	TDisk() = default;
	explicit TDisk(T radius);

	T getArea() const;

	TVector3<T> sampleToSurface(const std::array<T, 2>& sample) const;
	TVector3<T> sampleToSurface(const std::array<T, 2>& sample, T* out_pdfA) const;

	TVector2<T> sampleToSurface2D(const std::array<T, 2>& sample) const;
	TVector2<T> sampleToSurface2D(const std::array<T, 2>& sample, T* out_pdfA) const;

private:
	T m_radius;
};

// In-header Implementations:

template<typename T>
inline TDisk<T>::TDisk(const T radius) : 
	m_radius(radius)
{
	PH_ASSERT_GT(radius, T(0));
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
	return TVector3<T>(surfacePos2D.y, T(0), surfacePos2D.x);
}

template<typename T>
inline TVector3<T> TDisk<T>::sampleToSurface(const std::array<T, 2>& sample, T* const out_pdfA) const
{
	const auto surfacePos2D = sampleToSurface2D(sample, out_pdfA);
	return TVector3<T>(surfacePos2D.y, T(0), surfacePos2D.x);
}

template<typename T>
inline TVector2<T> TDisk<T>::sampleToSurface2D(const std::array<T, 2>& sample) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], T(0), T(1));
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], T(0), T(1));
	PH_ASSERT_GE(m_radius, T(0));

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
	// PDF is 1/(pi*r^2)
	PH_ASSERT(out_pdfA);
	*out_pdfA = T(1) / getArea();

	return sampleToSurface2D(sample);
}

}// end namespace ph::math
