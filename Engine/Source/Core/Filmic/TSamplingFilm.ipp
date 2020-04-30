#include "Core/Filmic/TSamplingFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilter.h"
#include "Common/assertion.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(
	const int64         actualWidthPx,
	const int64         actualHeightPx,
	const SampleFilter& filter) :
	
	TSamplingFilm(
		actualWidthPx, 
		actualHeightPx,
		math::TAABB2D<int64>(
			math::TVector2<int64>(0, 0),
			math::TVector2<int64>(actualWidthPx, actualHeightPx)),
		filter)
{}

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(
	const int64                 actualWidthPx,
	const int64                 actualHeightPx,
	const math::TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter&         filter) :

	Film(
		actualWidthPx,
		actualHeightPx,
		effectiveWindowPx),

	m_filter(filter)
{
	setSoftEdge(true);

	PH_ASSERT(getSampleWindowPx().isValid());
}

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(TSamplingFilm&& other) :

	Film(std::move(other)),

	m_filter        (std::move(other.m_filter)),
	m_sampleWindowPx(std::move(other.m_sampleWindowPx)),
	m_useSoftEdge   (other.m_useSoftEdge)
{}

template<typename Sample>
inline void TSamplingFilm<Sample>::setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow)
{
	Film::setEffectiveWindowPx(effectiveWindow);

	updateSampleDimensions();
}

template<typename Sample>
inline TSamplingFilm<Sample>& TSamplingFilm<Sample>::operator = (TSamplingFilm&& other)
{
	Film::operator = (std::move(other));

	m_filter         = std::move(other.m_filter);
	m_sampleWindowPx = std::move(other.m_sampleWindowPx);
	m_useSoftEdge    = other.m_useSoftEdge;

	return *this;
}

template<typename Sample>
inline void TSamplingFilm<Sample>::updateSampleDimensions()
{
	if(m_useSoftEdge)
	{
		m_sampleWindowPx = math::TAABB2D<float64>(
			math::TVector2<float64>(getEffectiveWindowPx().getMinVertex()).add(0.5).sub(m_filter.getHalfSizePx()),
			math::TVector2<float64>(getEffectiveWindowPx().getMaxVertex()).sub(0.5).add(m_filter.getHalfSizePx()));

		if(!m_sampleWindowPx.isValid())
		{
			std::cerr << "warning: at TSamplingFilm::updateSampleDimensions(), "
			          << "invalid sampling window detected" << std::endl;
		}
	}
	else
	{
		m_sampleWindowPx = math::TAABB2D<float64>(getEffectiveWindowPx());
	}
}

}// end namespace ph
