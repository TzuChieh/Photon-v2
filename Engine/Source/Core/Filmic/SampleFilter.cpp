#include "Core/Filmic/SampleFilter.h"

namespace ph
{

SampleFilter::SampleFilter(std::unique_ptr<TMathFunction2D<float64>> filter,
                           const float64 widthPx, const float64 heightPx) :
	m_filter(std::move(filter)),
	m_widthPx(widthPx), m_heightPx(heightPx),
	m_halfWidthPx(widthPx * 0.5), m_halfHeightPx(heightPx * 0.5)
{

}

SampleFilter::SampleFilter(SampleFilter&& other) : 
	m_filter(std::move(other.m_filter)),
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_halfWidthPx(other.m_halfWidthPx), m_halfHeightPx(other.m_halfHeightPx)
{

}

float64 SampleFilter::evaluate(const float64 xPx, const float64 yPx)
{
	return m_filter->evaluate(xPx, yPx);
}

SampleFilter& SampleFilter::operator = (SampleFilter&& rhs)
{
	m_filter = std::move(rhs.m_filter);
	m_widthPx  = rhs.m_widthPx;
	m_heightPx = rhs.m_heightPx;
	m_halfWidthPx  = rhs.m_halfWidthPx;
	m_halfHeightPx = rhs.m_halfHeightPx;
}

}// end namespace ph