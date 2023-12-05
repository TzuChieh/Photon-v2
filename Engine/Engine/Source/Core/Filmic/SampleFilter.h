#pragma once

#include "Math/Function/TMathFunction2D.h"
#include "Math/TVector2.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class SampleFilter final
{
public:
	SampleFilter();

	SampleFilter(
		const std::shared_ptr<math::TMathFunction2D<float64>>& filterFunc,
		float64 widthPx,
		float64 heightPx);

	float64 evaluate(float64 xPx, float64 yPx) const;
	const math::TVector2<float64>& getSizePx() const;
	const math::TVector2<float64>& getHalfSizePx() const;

private:
	std::shared_ptr<math::TMathFunction2D<float64>> m_filterFunc;
	math::TVector2<float64> m_sizePx;
	math::TVector2<float64> m_halfSizePx;
};

inline const math::TVector2<float64>& SampleFilter::getSizePx() const
{
	return m_sizePx;
}

inline const math::TVector2<float64>& SampleFilter::getHalfSizePx() const
{
	return m_halfSizePx;
}

}// end namespace ph
