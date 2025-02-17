#pragma once

#include "Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <type_traits>
#include <vector>

namespace ph::math
{

/*! @brief A piecewise constant distribution of floating-point type `T`.
As `TPiecewiseConstantDistribution<X>D`, an alias table is useful for generating samples
according to the specified distribution. Alias table can offer @f$ O(1) @f$ sample generation;
however, more memory is used for achieving this kind of efficiency.
*/
template<typename T, typename Index = std::size_t>
class TAliasTable final
{
	static_assert(std::is_floating_point_v<T>);

	// OPT: sort weights for better performance (interface for index remapping)

public:
	/*! @brief Constructs a distribution.
	The range of the distribution is [`min`, `max`]; and corresponding sample
	weight of each column is specified via `weights`.
	*/
	TAliasTable(T min, T max, TSpanView<T> weights);

	/*! @brief Constructs a distribution with range [0, 1].
	*/
	explicit TAliasTable(TSpanView<T> weights);

	TAliasTable();

	/*! @brief Generate a continuous sample.
	Given a uniform unit random sample, generate a continuous sample according to
	the sample weights.
	*/
	///@{
	T sampleContinuous(T sample) const;
	T sampleContinuous(T sample, T* out_pdf) const;
	T sampleContinuous(T sample, T* out_pdf, std::size_t* out_straddledColumn) const;
	///@}

	/*! @brief Generate an index.
	Given a uniform unit random sample, generate a column index according to the sample weights.
	@return Column index in [0, `numColumns()`).
	*/
	std::size_t sampleDiscrete(T sample) const;
	// TODO: a sample resued version

	/*!
	@return PDF of a continuously sampled value.
	*/
	T pdfContinuous(T value) const;

	/*!
	@return PDF of a continuous sample given its corresponding column index.
	*/
	T pdfContinuous(std::size_t columnIndex) const;

	/*!
	@return PDF of a discrete sample.
	*/
	T pdfDiscrete(std::size_t columnIndex) const;

	/*! @brief Calculates the sampled column index given a continuously sampled value.
	@return The sampled column index.
	*/
	std::size_t continuousToDiscrete(T value) const;

	/*!
	@return The number of sample weights originally provided.
	*/
	std::size_t numColumns() const;

private:
	// An entry of the table. We store `aliasedPdf` for memory locality, though we could use
	// `aliasedIdx` to reference the aliased PDF to reduce memory usage.
	struct Entry final
	{
		T tau;
		T pdf;
		T aliasedPdf;
		Index aliasedIdx;
	};

	// Range of the distribution
	T m_min, m_max;

	// Size of each interval
	T m_delta;

	// Piecewise constant weights will result in piecewise linear CDF.
	// CDF values are stored on all turning points of the function. 
	std::vector<T> m_cdf;

	T continuouslySampleValue(T sample, std::size_t straddledColumn) const;
};

}// end namespace ph::math

#include "Math/Function/Distribution/TAliasTable.ipp"
