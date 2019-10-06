#pragma once

#include "Common/primitive_type.h"
#include "Math/Function/TMathFunction2D.h"
#include "Math/TVector2.h"

#include <memory>

namespace ph
{

class SampleFilter final
{
public:
	SampleFilter() = default;
	SampleFilter(const std::shared_ptr<TMathFunction2D<float64>>& filter,
	             float64 widthPx, float64 heightPx);
	SampleFilter(const SampleFilter& other);
	SampleFilter(SampleFilter&& other);

	float64 evaluate(float64 xPx, float64 yPx) const;

	inline const TVector2<float64>& getSizePx() const
	{
		return m_sizePx;
	}

	inline const TVector2<float64>& getHalfSizePx() const
	{
		return m_halfSizePx;
	}

	SampleFilter& operator = (const SampleFilter& rhs);

private:
	std::shared_ptr<TMathFunction2D<float64>> m_filter;
	TVector2<float64>                         m_sizePx;
	TVector2<float64>                         m_halfSizePx;
};

}// end namespace ph
