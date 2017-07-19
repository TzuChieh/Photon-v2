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
	SampleFilter(std::unique_ptr<TMathFunction2D<float64>> filter,
	             float64 widthPx, float64 heightPx);
	SampleFilter(SampleFilter&& other);

	float64 evaluate(float64 xPx, float64 yPx);

	SampleFilter& operator = (SampleFilter&& rhs);

	inline const TVector2<float64>& getSizePx() const
	{
		return m_sizePx;
	}

	inline const TVector2<float64>& getHalfSizePx() const
	{
		return m_halfSizePx;
	}

	// forbid copying
	SampleFilter(const SampleFilter& other) = delete;
	SampleFilter& operator = (const SampleFilter& rhs) = delete;

private:
	std::unique_ptr<TMathFunction2D<float64>> m_filter;
	TVector2<float64> m_sizePx;
	TVector2<float64> m_halfSizePx;
};

}// end namespace ph