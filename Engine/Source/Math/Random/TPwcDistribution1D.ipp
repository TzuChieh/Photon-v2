#pragma once

#include "Math/Random/TPwcDistribution1D.h"
#include "Common/assertion.h"
#include "Math/Math.h"

#include <algorithm>

namespace ph
{

template<typename T>
inline TPwcDistribution1D<T>::TPwcDistribution1D(
	const T min, const T max,
	const std::vector<T>& weights) :
	TPwcDistribution1D(min, max, weights.data(), weights.size())
{}

template<typename T>
inline TPwcDistribution1D<T>::TPwcDistribution1D(
	const T           min, 
	const T           max,
	const T* const    weights,
	const std::size_t numWeights) :

	m_min(min), m_max(max),

	m_delta(0),

	m_firstNonZeroPdfColumn(0),

	// one more entry since we are storing values on endpoints
	m_cdf(numWeights + 1, 0)
{
	PH_ASSERT(max > min && weights && numWeights > 0);
	m_delta = (max - min) / static_cast<T>(numWeights);

	// construct CDF by first integrating the weights
	m_cdf[0] = 0;
	for(std::size_t i = 1; i < m_cdf.size(); ++i)
	{
		const T wi = weights[i - 1];
		PH_ASSERT(wi >= 0);

		m_cdf[i] = m_cdf[i - 1] + wi * m_delta;
	}

	const T sum = m_cdf.back();
	if(sum > 0)
	{
		// normalize the CDF
		for(std::size_t i = 1; i < m_cdf.size(); ++i)
		{
			// We do not multiply with reciprocal of sum here since we want to
			// ensure that trailing entries with zero weight are normalized
			// to one.
			m_cdf[i] /= sum;
		}
		PH_ASSERT(m_cdf.back() == 1);
	}
	else
	{
		PH_ASSERT(sum == 0);

		// If the sum is zero, make a simple uniform CDF.
		for(std::size_t i = 1; i < m_cdf.size(); ++i)
		{
			m_cdf[i] = static_cast<T>(i) / static_cast<T>(m_cdf.size());
		}
	}

	// find first column with non-zero PDF
	for(std::size_t i = 0; i < numColumns(); ++i)
	{
		if(pdf(i) > 0)
		{
			m_firstNonZeroPdfColumn = i;
			break;
		}
	}
}

template<typename T>
inline TPwcDistribution1D<T>::TPwcDistribution1D(const std::vector<T>& weights) :
	TPwcDistribution1D(0, 1, weights)
{}

template<typename T>
inline std::size_t TPwcDistribution1D<T>::sampleDiscrete(const T seed_i0_e1) const
{
	const auto& result = std::lower_bound(m_cdf.begin(), m_cdf.end(), seed_i0_e1);
	PH_ASSERT(result != m_cdf.end());

	return result != m_cdf.begin() ? result - m_cdf.begin() - 1 : m_firstNonZeroPdfColumn;
}

template<typename T>
inline T TPwcDistribution1D<T>::sampleContinuous(const T seed_i0_e1) const
{
	const std::size_t sampledColumn = sampleDiscrete(seed_i0_e1);
	return calcContinuousSample(seed_i0_e1, sampledColumn);
}

template<typename T>
inline T TPwcDistribution1D<T>::sampleContinuous(const T seed_i0_e1, T* const out_pdf) const
{
	PH_ASSERT(out_pdf);

	const std::size_t sampledColumn = sampleDiscrete(seed_i0_e1);

	*out_pdf = pdf(sampledColumn);
	return calcContinuousSample(seed_i0_e1, sampledColumn);
}

template<typename T>
inline std::size_t TPwcDistribution1D<T>::numColumns() const
{
	PH_ASSERT(m_cdf.size() >= 2);

	return m_cdf.size() - 1;
}

template<typename T>
inline T TPwcDistribution1D<T>::pdf(const std::size_t columnIndex) const
{
	PH_ASSERT(!m_cdf.empty() && 
	          0 <= columnIndex && columnIndex < numColumns());

	return m_cdf[columnIndex + 1] - m_cdf[columnIndex];
}

template<typename T>
inline T TPwcDistribution1D<T>::calcContinuousSample(const T seed_i0_e1, const std::size_t straddledColumn) const
{
	PH_ASSERT(straddledColumn < numColumns());

	const T cdfDelta = m_cdf[straddledColumn + 1] - m_cdf[straddledColumn];
	T overshoot      = seed_i0_e1 - m_cdf[straddledColumn];
	if(cdfDelta > 0)
	{
		overshoot /= cdfDelta;
	}
	PH_ASSERT(0 <= overshoot && overshoot <= 1);

	// NOTE: <sample> may have value straddling neighbor column's range due to
	// numerical error. Currently this is considered acceptable since continuous
	// sample does not require precise result.
	const T sample = m_delta * (overshoot + static_cast<T>(straddledColumn));

	// TODO: check rare, sample should rarely exceed [min, max]
	return Math::clamp(sample, m_min, m_max);
}

}// end namespace ph