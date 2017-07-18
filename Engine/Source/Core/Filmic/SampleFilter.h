#pragma once

#include "Common/primitive_type.h"
#include "Math/Function/TMathFunction2D.h"

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

	inline float64 getWidthPx() const
	{
		return m_widthPx;
	}

	inline float64 getHeightPx() const
	{
		return m_heightPx;
	}

	inline float64 getHalfWidthPx() const
	{
		return m_halfWidthPx;
	}

	inline float64 getHalfHeightPx() const
	{
		return m_halfHeightPx;
	}

	SampleFilter& operator = (SampleFilter&& rhs);

	// forbid copying
	SampleFilter(const SampleFilter& other) = delete;
	SampleFilter& operator = (const SampleFilter& rhs) = delete;

private:
	std::unique_ptr<TMathFunction2D<float64>> m_filter;
	float64 m_widthPx;
	float64 m_heightPx;
	float64 m_halfWidthPx;
	float64 m_halfHeightPx;
};

}// end namespace ph