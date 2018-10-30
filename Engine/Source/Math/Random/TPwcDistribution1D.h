#pragma once

#include <type_traits>
#include <vector>

namespace ph
{

/*
	A 1-D piecewise constant distribution of floating-point type <T>. The 
	sample weights can be seen as a histogram, and samples are drawn according
	to each column's relative heights. Each generated sample is guaranteed to
	have a non-zero PDF.
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

	TPwcDistribution1D();

	// Given a uniform random seed in [0, 1), generates a continuous sample 
	// according to the sample weights.
	T sampleContinuous(T seed_i0_e1) const;
	T sampleContinuous(T seed_i0_e1, T* out_pdf) const;
	T sampleContinuous(T seed_i0_e1, T* out_pdf, std::size_t* out_straddledColumn) const;

	// Given a uniform random seed in [0, 1), generates a column index 
	// according to the sample weights.
	std::size_t sampleDiscrete(T seed_i0_e1) const;

	// PDF of a continuous sample.
	T pdfContinuous(T sample) const;

	// PDF of a continuous sample given its corresponding column index.
	T pdfContinuous(std::size_t columnIndex) const;

	// PDF of a discrete sample.
	T pdfDiscrete(std::size_t columnIndex) const;

	// Calculates the sampled column index given a continuous sample.
	std::size_t continuousToDiscrete(T sample) const;

	// Gets the number of sample weights originally provided.
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