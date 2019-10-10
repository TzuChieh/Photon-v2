#pragma once

#include "Math/Random/TPwcDistribution2D.h"
#include "Common/assertion.h"

namespace ph::math
{

template<typename T>
inline TPwcDistribution2D<T>::TPwcDistribution2D(
	const TAABB2D<T>&            range,
	const T* const               weights,
	const TVector2<std::size_t>& numWeights) : 

	m_marginalYs(),
	m_conditionalXs(numWeights.y)
{
	PH_ASSERT(weights && numWeights.x > 0 && numWeights.y > 0);

	// initialize conditional distributions for each row
	std::vector<T> rowSums(numWeights.y, 0);
	for(std::size_t y = 0; y < numWeights.y; ++y)
	{
		const std::size_t baseIndex = y * numWeights.x;
		for(std::size_t x = 0; x < numWeights.x; ++x)
		{
			rowSums[y] += weights[baseIndex + x];
		}

		m_conditionalXs[y] = TPwcDistribution1D<T>(
			range.minVertex.x, 
			range.maxVertex.x, 
			&(weights[baseIndex]),
			numWeights.x);
	}

	// initialize marginal distribution for each row
	m_marginalYs = TPwcDistribution1D<T>(
		range.minVertex.y,
		range.maxVertex.y, 
		rowSums);
}

template<typename T>
inline TPwcDistribution2D<T>::TPwcDistribution2D(
	const T* const               weights,
	const TVector2<std::size_t>& numWeights) : 

	TPwcDistribution2D(
		TAABB2D<T>(TVector2<T>(0), TVector2<T>(1)), 
		weights, 
		numWeights)
{}

template<typename T>
inline TPwcDistribution2D<T>::TPwcDistribution2D() = default;

template<typename T>
inline TVector2<T> TPwcDistribution2D<T>::sampleContinuous(
	const T  seedX_i0_e1, 
	const T  seedY_i0_e1, 
	T* const out_pdf) const
{
	PH_ASSERT(out_pdf);

	std::size_t y;
	T pdfY, pdfXgivenY;
	const T sampleY = m_marginalYs.sampleContinuous(seedY_i0_e1, &pdfY, &y);
	const T sampleX = m_conditionalXs[y].sampleContinuous(seedX_i0_e1, &pdfXgivenY);

	*out_pdf = pdfXgivenY * pdfY;
	PH_ASSERT(*out_pdf > 0);
	return TVector2<T>(sampleX, sampleY);
}

template<typename T>
inline real TPwcDistribution2D<T>::pdfContinuous(const TVector2<T>& sample) const
{
	const std::size_t y = m_marginalYs.continuousToDiscrete(sample.y);

	const T pdfY       = m_marginalYs.pdfContinuous(y);
	const T pdfXgivenY = m_conditionalXs[y].pdfContinuous(sample.x);

	return pdfXgivenY * pdfY;
}

}// end namespace ph::math
