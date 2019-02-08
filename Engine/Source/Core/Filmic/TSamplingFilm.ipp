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
		TAABB2D<int64>(
			TVector2<int64>(0, 0),
			TVector2<int64>(actualWidthPx, actualHeightPx)),
		filter)
{}

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(
	const int64           actualWidthPx,
	const int64           actualHeightPx,
	const TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter&   filter) :

	Film(
		actualWidthPx,
		actualHeightPx,
		effectiveWindowPx),

	m_filter(filter)
{
	updateSampleDimensions();

	PH_ASSERT(getSampleWindowPx().isValid());
}

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(TSamplingFilm&& other) :

	Film(std::move(other)),

	m_filter        (std::move(other.m_filter)),
	m_sampleWindowPx(std::move(other.m_sampleWindowPx))
{
	PH_ASSERT(getSampleWindowPx().isValid());
}

template<typename Sample>
inline void TSamplingFilm<Sample>::setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow)
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

	return *this;
}

template<typename Sample>
inline void TSamplingFilm<Sample>::updateSampleDimensions()
{
	m_sampleWindowPx = TAABB2D<float64>(
		TVector2<float64>(getEffectiveWindowPx().minVertex).add(0.5).sub(m_filter.getHalfSizePx()),
		TVector2<float64>(getEffectiveWindowPx().maxVertex).sub(0.5).add(m_filter.getHalfSizePx()));

	if(!m_sampleWindowPx.isValid())
	{
		std::cerr << "warning: at TSamplingFilm::updateSampleDimensions(), "
		          << "invalid sampling window detected" << std::endl;
	}
}

}// end namespace ph