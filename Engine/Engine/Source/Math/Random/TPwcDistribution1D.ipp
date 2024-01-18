#pragma once

#include "Math/Random/TPwcDistribution1D.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <algorithm>

namespace ph::math
{

template<typename T>
inline TPwcDistribution1D<T>::TPwcDistribution1D(
	const T min, const T max,
	const std::vector<T>& weights) :

	TPwcDistribution1D(
		min, max, 
		weights.data(), weights.size())
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

	// One more entry since we are storing values on endpoints
	m_cdf(numWeights + 1, 0)
{
	PH_ASSERT(max > min && weights && numWeights > 0);
	m_delta = (max - min) / static_cast<T>(numWeights);

	// Construct CDF by first integrating the weights
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
		// Normalize the CDF
		for(std::size_t i = 1; i < m_cdf.size(); ++i)
		{
			// We do not multiply with reciprocal of sum here since we want to
			// ensure that trailing entries with zero weight are normalized
			// to one.
			m_cdf[i] /= sum;
		}
	}
	else
	{
		PH_ASSERT_EQ(sum, T(0));

		// If the sum is zero, make a simple linear CDF.
		for(std::size_t i = 1; i < m_cdf.size(); ++i)
		{
			m_cdf[i] = static_cast<T>(i) / static_cast<T>(numWeights);
		}
	}
	PH_ASSERT_EQ(m_cdf.back(), T(1));

	// Find first column with non-zero PDF
	for(std::size_t i = 0; i < numColumns(); ++i)
	{
		if(pdfContinuous(i) > 0)
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
inline TPwcDistribution1D<T>::TPwcDistribution1D() = default;

template<typename T>
inline std::size_t TPwcDistribution1D<T>::sampleDiscrete(const T sample) const
{
	const auto& result = std::lower_bound(m_cdf.begin(), m_cdf.end(), sample);
	PH_ASSERT_MSG(result != m_cdf.end(), 
		"sample = " + std::to_string(sample) + ", "
		"last CDF value = " + (m_cdf.empty() ? "(empty CDF)" : std::to_string(m_cdf.back())));

	return result != m_cdf.begin() ? result - m_cdf.begin() - 1 : m_firstNonZeroPdfColumn;
}

template<typename T>
inline T TPwcDistribution1D<T>::sampleContinuous(const T sample) const
{
	const std::size_t sampledColumn = sampleDiscrete(sample);
	return continuouslySampleValue(sample, sampledColumn);
}

template<typename T>
inline T TPwcDistribution1D<T>::sampleContinuous(const T sample, T* const out_pdf) const
{
	PH_ASSERT(out_pdf);

	const std::size_t sampledColumn = sampleDiscrete(sample);

	*out_pdf = pdfContinuous(sampledColumn);
	return continuouslySampleValue(sample, sampledColumn);
}

template<typename T>
inline T TPwcDistribution1D<T>::sampleContinuous(
	const T            sample,
	T* const           out_pdf, 
	std::size_t* const out_straddledColumn) const
{
	PH_ASSERT(out_pdf);
	PH_ASSERT(out_straddledColumn);

	*out_straddledColumn = sampleDiscrete(sample);
	*out_pdf             = pdfContinuous(*out_straddledColumn);
	return continuouslySampleValue(sample, *out_straddledColumn);
}

template<typename T>
inline std::size_t TPwcDistribution1D<T>::numColumns() const
{
	PH_ASSERT(m_cdf.size() >= 2);

	return m_cdf.size() - 1;
}

template<typename T>
inline T TPwcDistribution1D<T>::pdfContinuous(const T sample) const
{
	return pdfContinuous(continuousToDiscrete(sample));
}

template<typename T>
inline T TPwcDistribution1D<T>::pdfContinuous(const std::size_t columnIndex) const
{
	PH_ASSERT(!m_cdf.empty() && 
	          0 <= columnIndex && columnIndex < numColumns());

	return (m_cdf[columnIndex + 1] - m_cdf[columnIndex]) / m_delta;
}

template<typename T>
inline T TPwcDistribution1D<T>::pdfDiscrete(const std::size_t columnIndex) const
{
	PH_ASSERT(!m_cdf.empty() && 
	          0 <= columnIndex && columnIndex < numColumns());

	return m_cdf[columnIndex + 1] - m_cdf[columnIndex];
}

template<typename T>
std::size_t TPwcDistribution1D<T>::continuousToDiscrete(const T sample) const
{
	PH_ASSERT_MSG(m_min <= sample && sample <= m_max,
		"m_min = "  + std::to_string(m_min) + ", "
		"m_max = "  + std::to_string(m_max) + ", "
		"sample = " + std::to_string(sample));

	const T continuousColumn = (sample - m_min) / m_delta;
	return math::clamp(static_cast<std::size_t>(continuousColumn),
	                   static_cast<std::size_t>(0), numColumns() - 1);
}

template<typename T>
inline T TPwcDistribution1D<T>::continuouslySampleValue(const T sample, const std::size_t straddledColumn) const
{
	PH_ASSERT(straddledColumn < numColumns());

	const T cdfDelta = m_cdf[straddledColumn + 1] - m_cdf[straddledColumn];
	T overshoot      = sample - m_cdf[straddledColumn];
	if(cdfDelta > 0)
	{
		overshoot /= cdfDelta;
	}
	PH_ASSERT(0 <= overshoot && overshoot <= 1);

	// NOTE: <sampledValue> may have value straddling neighbor column's range 
	// due to numerical error. Currently this is considered acceptable since 
	// continuous sample does not require precise result.
	const T sampledValue = m_delta * (overshoot + static_cast<T>(straddledColumn));

	// TODO: check rare, sampled value should rarely exceed [min, max]
	return math::clamp(sampledValue, m_min, m_max);
}

}// end namespace ph::math
