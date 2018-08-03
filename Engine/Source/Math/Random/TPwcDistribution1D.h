#pragma once

#include <type_traits>
#include <vector>

namespace ph
{

/*
	A 1-D piecewise constant distribution of floating-point type <T>.
*/
template<typename T>
class TPwcDistribution1D
{
	static_assert(std::is_floating_point_v<T>);

public:
	// The range of the distribution is[<min>, <max>]; and corresponding sample
	// weight of each column is specified via <weights>.
	TPwcDistribution1D(T min, T max, const T* weights, std::size_t numWeights);
	TPwcDistribution1D(T min, T max, const std::vector<T>& weights);

	// Constructs a distribution with range [0, 1].
	explicit TPwcDistribution1D(const std::vector<T>& weights);

	std::size_t sampleDiscrete(T seed_i0_e1) const;
	T sampleContinuous(T seed_i0_e1) const;
	T sampleContinuous(T seed_i0_e1, T* out_pdf) const;
	T pdf(std::size_t columnIndex) const;
	std::size_t numColumns() const;

private:
	// range of the distribution
	T m_min, m_max;

	// size of each interval
	T m_delta;

	// Recording first non-zero column index to avoid sampling column with
	// zero PDF.
	std::size_t m_firstNonZeroPdfColumn;

	// Piecewise constant weights will result in piecewise linear CDF.
	// CDF values are stored on all turning points of the function. 
	std::vector<T> m_cdf;

	T calcContinuousSample(T seed_i0_e1, std::size_t straddledColumn) const;
};

}// end namespace ph

#include "Math/Random/TPwcDistribution1D.ipp"