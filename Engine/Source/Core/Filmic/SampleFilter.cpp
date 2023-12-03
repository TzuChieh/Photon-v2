#include "Core/Filmic/SampleFilter.h"
#include "Common/assertion.h"

namespace ph
{

SampleFilter::SampleFilter()
	: m_filterFunc(nullptr)
	, m_sizePx(0)
	, m_halfSizePx(0)
{}

SampleFilter::SampleFilter(
	const std::shared_ptr<math::TMathFunction2D<float64>>& filterFunc,
	const float64 widthPx, 
	const float64 heightPx)

	: m_filterFunc(filterFunc)
	, m_sizePx(widthPx, heightPx)
	, m_halfSizePx(widthPx * 0.5, heightPx * 0.5)
{
	PH_ASSERT(m_filterFunc);
}

float64 SampleFilter::evaluate(const float64 xPx, const float64 yPx) const
{
	return m_filterFunc->evaluate(xPx, yPx);
}

}// end namespace ph
