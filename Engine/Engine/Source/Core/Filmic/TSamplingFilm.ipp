#include "Core/Filmic/TSamplingFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilter.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(
	const int64         actualWidthPx,
	const int64         actualHeightPx,
	const SampleFilter& filter)
	
	: TSamplingFilm(
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
	const SampleFilter&         filter)

	: Film(
		actualWidthPx,
		actualHeightPx,
		effectiveWindowPx)

	, m_filter(filter)
	, m_sampleWindowPx(math::TAABB2D<float64>::makeEmpty())
	, m_softness(1.0f)
{
	setSoftEdge(true);

	PH_ASSERT(!getSampleWindowPx().isEmpty());
}

template<typename Sample>
inline void TSamplingFilm<Sample>::setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow)
{
	Film::setEffectiveWindowPx(effectiveWindow);

	updateSampleDimensions();
}

template<typename Sample>
inline void TSamplingFilm<Sample>::setSoftEdge(const bool useSoftEdge, const float32 softness)
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(softness, 0.0f, 1.0f);
	m_softness = useSoftEdge ? softness : 0.0f;

	updateSampleDimensions();
}

template<typename Sample>
inline const SampleFilter& TSamplingFilm<Sample>::getFilter() const
{
	return m_filter;
}

template<typename Sample>
inline math::TVector2<float64> TSamplingFilm<Sample>::getSampleResPx() const
{
	return {m_sampleWindowPx.getWidth(), m_sampleWindowPx.getHeight()};
}

template<typename Sample>
inline auto TSamplingFilm<Sample>::getSampleWindowPx() const
	-> const math::TAABB2D<float64>&
{
	return m_sampleWindowPx;
}

template<typename Sample>
inline SamplingFilmDimensions TSamplingFilm<Sample>::getDimensions() const
{
	return {getActualResPx(), getEffectiveWindowPx(), getSampleWindowPx()};
}

template<typename Sample>
inline bool TSamplingFilm<Sample>::isSoftEdged() const
{
	return m_softness > 0.0f;
}

template<typename Sample>
inline void TSamplingFilm<Sample>::updateSampleDimensions()
{
	// Softness = 1: full filter half size; 0: nothing
	const auto pxToExpand = math::Vector2D(m_filter.getHalfSizePx()) * m_softness;

	// Expand from pixel centers (discrete coordinates + 0.5) 
	m_sampleWindowPx = math::TAABB2D<float64>(
		math::Vector2D(getEffectiveWindowPx().getMinVertex()).add(0.5).sub(pxToExpand),
		math::Vector2D(getEffectiveWindowPx().getMaxVertex()).sub(0.5).add(pxToExpand));

	if(m_sampleWindowPx.isEmpty())
	{
		PH_DEFAULT_LOG_WARNING(
			"Sampling film has empty sample window (effective window = {}, half filter size = {}, "
			"softness = {}).", getEffectiveWindowPx().toString(), getFilter().getHalfSizePx(), m_softness);
	}

	// Even if softness is 0, sample window will not necessarily equal to effective window. Sample
	// window is the region where samples will contribute to the film, and that depends on the
	// filter size. Only filters of unit size (with softness = 0) will make sample window equal
	// to effective window.
#if PH_DEBUG
	if(m_softness == 0.0f && m_filter.getHalfSizePx().isEqual({0.5, 0.5}))
	{
		// For fairly small integers and only do +- of 0.5, no numeric error should manifest
		const auto fEffectiveWindowPx = math::TAABB2D<float64>(getEffectiveWindowPx());
		PH_ASSERT_MSG(m_sampleWindowPx == fEffectiveWindowPx,
			"sample window = " + m_sampleWindowPx.toString() + ", "
			"effective window = " + fEffectiveWindowPx.toString());
	}
#endif
}

}// end namespace ph
