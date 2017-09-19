#include "Core/Filmic/SampleFilter.h"

namespace ph
{

SampleFilter::SampleFilter(std::unique_ptr<TMathFunction2D<float64>> filter,
                           const float64 widthPx, const float64 heightPx) :
	m_filter(std::move(filter)),
	m_sizePx(widthPx, heightPx),
	m_halfSizePx(widthPx * 0.5, heightPx * 0.5)
{

}

SampleFilter::SampleFilter(SampleFilter&& other) : 
	m_filter(std::move(other.m_filter)),
	m_sizePx(other.m_sizePx), 
	m_halfSizePx(other.m_halfSizePx)
{

}

float64 SampleFilter::evaluate(const float64 xPx, const float64 yPx)
{
	return m_filter->evaluate(xPx, yPx);
}

SampleFilter& SampleFilter::operator = (SampleFilter&& rhs)
{
	m_filter     = std::move(rhs.m_filter);
	m_sizePx     = rhs.m_sizePx;
	m_halfSizePx = rhs.m_halfSizePx;

	return *this;
}

}// end namespace ph