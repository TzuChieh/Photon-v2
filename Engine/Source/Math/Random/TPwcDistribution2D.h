#pragma once

#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Math/Random/TPwcDistribution1D.h"

#include <vector>

namespace ph
{

template<typename T>
class TPwcDistribution2D
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
	TVector2<T> sampleContinuous(T seedX_i0_e1, T seedY_i0_e1, T* out_pdf) const;

	real pdf(const TVector2<T>& sample) const;

private:
	TPwcDistribution1D<T>              m_marginalYs;
	std::vector<TPwcDistribution1D<T>> m_conditionalXs;
};

}// end namespace ph

#include "Math/Random/TPwcDistribution2D.ipp"