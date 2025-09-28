#pragma once

#include "Engine/Utility/TSpan.h"

#include <Common/assertion.h>

#include <cstddef>
#include <cmath>
#include <concepts>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <utility>

namespace ph::math
{

/*!
When performing multiple statistical tests, the chance of finding a statistically significant result
by random increases with each additional test. For example, if you perform 10 independent tests
with a standard significance level of 0.05, the probability of getting at least one false positive
result will be much higher than 5%. Sidak's correction account for this by adjusting the original
significance level such that the overall significance level remains at the desired level.
See https://en.wikipedia.org/wiki/%C5%A0id%C3%A1k_correction for a formal description and a concise
mathematical derivation. This correction is exact for independent tests.
@param desiredProb Desired overall significance level.
@param numTests Number of tests.
@return Adjusted significance level for each individual test.
*/
template<std::floating_point T, std::integral IntType>
inline T sidak_correction(T desiredProb, IntType numTests)
{
	PH_ASSERT_GE(numTests, 1);

	return 1 - std::pow(1 - desiredProb, 1 / static_cast<T>(numTests));
}

/*! @brief Computes regularized lower incomplete gamma function.
"A Set of Algorithms for the Incomplete Gamma Functions" by N. M. Temme is also a good read on
this topic.
*/
template<std::floating_point T>
inline T regularized_lower_incomplete_gamma(const T a, const T x)
{
	// The following implementation is from pbrt-v4: https://github.com/mmp/pbrt-v4/blob/f140d7cba5dc7b941f9346d6b7d1476a05c28c37/src/pbrt/bsdfs_test.cpp#L56
	// Some changes are made to fit our style. it still computes in `double` no matter what `T` is.

	constexpr double epsilon = 0.000000000000001;
	constexpr double big     = 4503599627370496.0;
	constexpr double bigRcp  = 2.22044604925031308085e-16;

	if(a < 0 || x < 0)
	{
		throw std::runtime_error("`a` and `x` cannot be negative");
	}

	if(x == 0)
	{
		return 0;
	}

	double ax = (a * std::log(x)) - x - std::lgamma(a);
	if(ax < -709.78271289338399)
	{
		return a < x ? 1.0 : 0.0;
	}

	if(x <= 1 || x <= a)
	{
		double r2   = a;
		double c2   = 1;
		double ans2 = 1;

		do
		{
			r2 = r2 + 1;
			c2 = c2 * x / r2;
			ans2 += c2;
		}
		while((c2 / ans2) > epsilon);

		return std::exp(ax) * ans2 / a;
	}

	int    c   = 0;
	double y   = 1 - a;
	double z   = x + y + 1;
	double p3  = 1;
	double q3  = x;
	double p2  = x + 1;
	double q2  = z * x;
	double ans = p2 / q2;
	double error;
	do
	{
		++c;
		y += 1;
		z += 2;
		double yc = y * c;
		double p  = (p2 * z) - (p3 * yc);
		double q  = (q2 * z) - (q3 * yc);

		if(q != 0)
		{
			double nextAns = p / q;
			error = std::abs((ans - nextAns) / nextAns);
			ans = nextAns;
		}
		else
		{
			// Division by zero, skip
			error = 1;
		}

		// Shift
		p3 = p2;
		p2 = p;
		q3 = q2;
		q2 = q;

		// Normalize fraction when the numerator becomes large
		if(std::abs(p) > big)
		{
			p3 *= bigRcp;
			p2 *= bigRcp;
			q3 *= bigRcp;
			q2 *= bigRcp;
		}
	}
	while(error > epsilon);

	return static_cast<T>(1 - (std::exp(ax) * ans));
}

/*!
@param x Chi^2 value.
@param dof Degrees of freedom.
*/
template<std::floating_point T, std::integral DofType>
inline T chi2_CDF(T x, DofType dof)
{
	if(dof < 1 || x < 0)
	{
		return 0;
	}
	else if(dof == 2)
	{
		return static_cast<T>(1 - std::exp(-0.5 * x));
	}
	else
	{
		return static_cast<T>(regularized_lower_incomplete_gamma(0.5 * dof, 0.5 * x));
	}
}

/*!
If the distributions match, this is the probability of obtaining a Chi^2 value at least
as extreme as `x`.
*/
template<std::floating_point T, std::integral DofType>
inline T chi2_p_value(T x, DofType dof)
{
	return 1 - chi2_CDF(x, dof);
}

/*!
@return (chi^2, DoF).
*/
template<std::floating_point T, std::integral DofType>
inline std::pair<T, DofType> chi2(
	TSpanView<T> observedFreq,
	TSpanView<T> expectedFreq,
	T freqTolerance = 0,
	TSpan<std::size_t> poolingBuffer = {},
	T poolingFreq = 5,
	std::size_t* const numPooledBins = nullptr)
{
	PH_ASSERT_EQ(observedFreq.size(), expectedFreq.size());
	PH_ASSERT_GE(freqTolerance, 0);
	PH_ASSERT_GE(poolingFreq, 0);

	const bool usePooling = !poolingBuffer.empty();

	// If use pooling, this buffer stores the indices sorted according to expected frequencies
	if(usePooling)
	{
		PH_ASSERT_EQ(expectedFreq.size(), poolingBuffer.size());
		for(std::size_t binIdx = 0; binIdx < expectedFreq.size(); ++binIdx)
		{
			poolingBuffer[binIdx] = binIdx;
		}

		std::sort(
			poolingBuffer.begin(),
			poolingBuffer.end(),
			[&expectedFreq](std::size_t iA, std::size_t iB)
			{
				return expectedFreq[iA] < expectedFreq[iB];
			});
	}

	constexpr T invalidChi2 = std::numeric_limits<T>::has_infinity
		? std::numeric_limits<T>::infinity()
		: std::numeric_limits<T>::max();

	T pooledObservedFreq = 0;
	T pooledExpectedFreq = 0;
	T chi2 = 0;
	std::size_t dof = 0;

	if(numPooledBins)
	{
		*numPooledBins = 0;
	}

	// For each frequency bin
	for(std::size_t i = 0; i < expectedFreq.size(); ++i)
	{
		const std::size_t binIdx = usePooling ? poolingBuffer[i] : i;

		// If expected frequency is 0, a non-zero observed frequency will fail the test immediately
		// due to chi^2 being infinity. We allow a user specified tolerance so that small noises
		// won't fail the test.
		if(expectedFreq[binIdx] == 0)
		{
			if(observedFreq[binIdx] > freqTolerance)
			{
				return {invalidChi2, 0};
			}
		}
		// Low expected frequency are pooled to increase the statistical significance. A rule of
		// thumb is to set this threshold to 5, but it could be any value depending on the test.
		else if(usePooling && expectedFreq[binIdx] < poolingFreq)
		{
			pooledObservedFreq += observedFreq[binIdx];
			pooledExpectedFreq += expectedFreq[binIdx];

			if(numPooledBins)
			{
				++(*numPooledBins);
			}
		}
		// If pooled expected frequency is still too low, keep pooling (bins are sorted, so this bin
		// is the smallest among all remaining bins)
		else if(usePooling && pooledExpectedFreq > 0 && pooledExpectedFreq < poolingFreq)
		{
			pooledObservedFreq += observedFreq[binIdx];
			pooledExpectedFreq += expectedFreq[binIdx];

			if(numPooledBins)
			{
				++(*numPooledBins);
			}
		}
		else
		{
			PH_ASSERT_GE(observedFreq[binIdx], 0);
			PH_ASSERT_GT(expectedFreq[binIdx], 0);

			const T diff = observedFreq[binIdx] - expectedFreq[binIdx];
			chi2 += (diff * diff) / expectedFreq[binIdx];
			++dof;
		}
	}

	// DoF for the pool is 1, if any
	if(pooledObservedFreq > 0 || pooledExpectedFreq > 0)
	{
		PH_ASSERT_GT(pooledExpectedFreq, 0);

		const T diff = pooledObservedFreq - pooledExpectedFreq;
		chi2 += (diff * diff) / pooledExpectedFreq;
		++dof;
	}

	if(dof <= 0)
	{
		return {invalidChi2, 0};
	}
	dof -= 1;

	return {chi2, dof};
}

}// end namespace ph::math
