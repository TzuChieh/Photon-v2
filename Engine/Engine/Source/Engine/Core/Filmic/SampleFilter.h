#pragma once

#include "Engine/Math/Function/TMathFunction2D.h"
#include "Engine/Math/Function/TTabulatedMathFunction2D.h"
#include "Engine/Math/Geometry/TAABB2D.h"
#include "Engine/Math/TVector2.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <memory>
#include <utility>

namespace ph
{

/*! @brief An image reconstruction kernel.
*/
class SampleFilter final
{
public:
	/*! @brief Create sample filter from a math function.
	The filter may not be normalized over its support. Whether the filter is normalized is up to the
	given math function and its corresponding size.
	*/
	template<typename MathFunction>
	static SampleFilter make(MathFunction func, float64 widthPx, float64 heightPx, bool useTabulated);

	/*! @brief Create a normalized box filter.
	Box size is the same as a texel (1 px in width and height).
	*/
	static SampleFilter makeBox();

	/*! @brief Create a normalized Gaussian filter.
	*/
	static SampleFilter makeGaussian(bool useTabulated = true);

	/*! @brief Create a normalized Mitchell-Netravali filter.
	*/
	static SampleFilter makeMitchellNetravali(bool useTabulated = true);

	/*! @brief Create a normalized Blackman-Harris filter.
	*/
	static SampleFilter makeBlackmanHarris(bool useTabulated = true);

	SampleFilter();

	SampleFilter(
		std::shared_ptr<math::TMathFunction2D<float64>> filterFunc,
		float64 widthPx,
		float64 heightPx);

	float64 evaluate(float64 xPx, float64 yPx) const;

	/*!
	@return Width and height of the filter.
	*/
	const math::TVector2<float64>& getSizePx() const;

	/*!
	@return Half width and half height of the filter.
	*/
	const math::TVector2<float64>& getHalfSizePx() const;

private:
	// Best to be an odd number to capture centroid value
	static constexpr std::size_t TABULATED_FILTER_SIZE = 9;

	std::shared_ptr<math::TMathFunction2D<float64>> m_filterFunc;
	math::TVector2<float64> m_sizePx;
	math::TVector2<float64> m_halfSizePx;
};

template<typename MathFunction>
inline SampleFilter SampleFilter::make(
	MathFunction func,
	const float64 widthPx,
	const float64 heightPx,
	const bool useTabulated)
{
	if(useTabulated)
	{
		using TabulatedFilter = math::TTabulatedMathFunction2D<
			float64,
			TABULATED_FILTER_SIZE,
			TABULATED_FILTER_SIZE>;

		return SampleFilter(
			std::make_shared<TabulatedFilter>(
				func,
				math::TAABB2D<float64>(
					{-widthPx * 0.5, -heightPx * 0.5},
					{ widthPx * 0.5,  heightPx * 0.5})),
			widthPx,
			heightPx);
	}
	else
	{
		return SampleFilter(std::make_shared<MathFunction>(std::move(func)), widthPx, heightPx);
	}
}

inline float64 SampleFilter::evaluate(const float64 xPx, const float64 yPx) const
{
	return m_filterFunc->evaluate(xPx, yPx);
}

inline const math::TVector2<float64>& SampleFilter::getSizePx() const
{
	return m_sizePx;
}

inline const math::TVector2<float64>& SampleFilter::getHalfSizePx() const
{
	return m_halfSizePx;
}

}// end namespace ph
