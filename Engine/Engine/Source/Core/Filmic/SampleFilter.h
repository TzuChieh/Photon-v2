#pragma once

#include "Math/Function/TMathFunction2D.h"
#include "Math/TVector2.h"

#include <Common/primitive_type.h>

#include <memory>
#include <utility>

namespace ph
{

class SampleFilter final
{
public:
	/*! @brief Create sample filter from a math function.
	The filter may not be normalized over its support. Whether the filter is normalized is up to the
	given math function and its corresponding size.
	*/
	template<typename MathFunction>
	static SampleFilter make(MathFunction func, float64 widthPx, float64 heightPx);

	/*! @brief Create a normalized box filter.
	*/
	static SampleFilter makeBox();

	/*! @brief Create a normalized Gaussian filter.
	*/
	static SampleFilter makeGaussian();

	/*! @brief Create a normalized Mitchell-Netravali filter.
	*/
	static SampleFilter makeMitchellNetravali();

	/*! @brief Create a normalized Blackman-Harris filter.
	*/
	static SampleFilter makeBlackmanHarris();

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
	std::shared_ptr<math::TMathFunction2D<float64>> m_filterFunc;
	math::TVector2<float64> m_sizePx;
	math::TVector2<float64> m_halfSizePx;
};

template<typename MathFunction>
inline SampleFilter SampleFilter::make(MathFunction func, const float64 widthPx, const float64 heightPx)
{
	return SampleFilter(std::make_shared<MathFunction>(std::move(func)), widthPx, heightPx);
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
