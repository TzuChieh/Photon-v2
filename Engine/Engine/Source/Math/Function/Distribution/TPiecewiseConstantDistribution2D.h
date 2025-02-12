#pragma once

#include "Math/Geometry/TAABB2D.h"
#include "Math/TVector2.h"
#include "Math/Function/Distribution/TPiecewiseConstantDistribution1D.h"

#include <cstddef>
#include <vector>
#include <array>

namespace ph::math
{

template<typename T>
class TPiecewiseConstantDistribution2D final
{
public:
	TPiecewiseConstantDistribution2D(
		const TAABB2D<T>&            range, 
		const T*                     weights, 
		const TVector2<std::size_t>& numWeights);

	TPiecewiseConstantDistribution2D(
		const T*                     weights,
		const TVector2<std::size_t>& numWeights);

	TPiecewiseConstantDistribution2D();

	// PDFs returned by these methods are never zero.
	TVector2<T> sampleContinuous(const std::array<T, 2>& sample, T* out_pdf) const;

	T pdfContinuous(const std::array<T, 2>& sample) const;

private:
	TPiecewiseConstantDistribution1D<T>              m_marginalYs;
	std::vector<TPiecewiseConstantDistribution1D<T>> m_conditionalXs;
};

}// end namespace ph::math

#include "Math/Function/Distribution/TPiecewiseConstantDistribution2D.ipp"
