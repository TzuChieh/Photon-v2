#pragma once

#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

#include <cmath>
#include <array>

namespace ph::math
{

/*! @brief A 2-D disk with normal facing up (0, 1, 0).
*/
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

	TVector3<T> sampleToSurfaceOriented(
		const std::array<T, 2>& sample, 
		const TVector3<T>&      normal = TVector3<T>(0, 1, 0),
		const TVector3<T>&      offset = TVector3<T>(0)) const;

	TVector3<T> sampleToSurfaceOriented(
		const std::array<T, 2>& sample, 
		T*                      out_pdfA,
		const TVector3<T>&      normal = TVector3<T>(0, 1, 0),
		const TVector3<T>&      offset = TVector3<T>(0)) const;

private:
	T m_radius;

	T uniformSamplePdfA() const;
};

}// end namespace ph::math

#include "Math/Geometry/TDisk.ipp"
