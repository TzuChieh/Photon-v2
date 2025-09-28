#pragma once

#include "Engine/Math/Geometry/TAABB2D.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/Function/Distribution/TPiecewiseConstantDistribution1D.h"

#include <cstddef>
#include <vector>
#include <array>

namespace ph::math
{

template<typename T>
class TPiecewiseConstantDistribution2D final
{
public:
	/*!
	@param weights 2D array stored in row-major order.
	@param numWeights Stores `{numRows, numCols}`.
	*/
	TPiecewiseConstantDistribution2D(
		const TAABB2D<T>&            domain, 
		const T*                     weights, 
		const TVector2<std::size_t>& numWeights);

	/*!
	Unit-domain overload.
	*/
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

#include "Engine/Math/Function/Distribution/TPiecewiseConstantDistribution2D.ipp"
