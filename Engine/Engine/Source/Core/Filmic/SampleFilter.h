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
	template<typename MathFunction>
	static SampleFilter make(MathFunction func, float64 widthPx, float64 heightPx);

	static SampleFilter makeBox();
	static SampleFilter makeGaussian();
	static SampleFilter makeMitchellNetravali();
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

inline const math::TVector2<float64>& SampleFilter::getSizePx() const
{
	return m_sizePx;
}

inline const math::TVector2<float64>& SampleFilter::getHalfSizePx() const
{
	return m_halfSizePx;
}

}// end namespace ph
