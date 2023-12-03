#pragma once

#include "Math/Geometry/TAABB2D.h"
#include "Math/TVector2.h"
#include "Math/Random/TPwcDistribution1D.h"

#include <cstddef>
#include <vector>
#include <array>

namespace ph::math
{

template<typename T>
class TPwcDistribution2D final
{
public:
	TPwcDistribution2D(
		const TAABB2D<T>&            range, 
		const T*                     weights, 
		const TVector2<std::size_t>& numWeights);

	TPwcDistribution2D(
		const T*                     weights,
		const TVector2<std::size_t>& numWeights);

	TPwcDistribution2D();

	// PDFs returned by these methods are never zero.
	TVector2<T> sampleContinuous(const std::array<T, 2>& sample, T* out_pdf) const;

	T pdfContinuous(const std::array<T, 2>& sample) const;

private:
	TPwcDistribution1D<T>              m_marginalYs;
	std::vector<TPwcDistribution1D<T>> m_conditionalXs;
};

}// end namespace ph::math

#include "Math/Random/TPwcDistribution2D.ipp"
