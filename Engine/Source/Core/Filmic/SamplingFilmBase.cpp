#include "Core/Filmic/SamplingFilmBase.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilter.h"
#include "Common/assertion.h"
#include "Core/Filmic/SampleFilterFactory.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

SamplingFilmBase::SamplingFilmBase(
	const int64 actualWidthPx, const int64 actualHeightPx,
	const SampleFilter& filter) :

	SamplingFilmBase(
		actualWidthPx, actualHeightPx,
		TAABB2D<int64>(TVector2<int64>(0, 0),
		               TVector2<int64>(actualWidthPx, actualHeightPx)),
		filter)
{}

SamplingFilmBase::SamplingFilmBase(
	const int64 actualWidthPx, const int64 actualHeightPx,
	const TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter& filter) : 

	Film(actualWidthPx, actualHeightPx, effectiveWindowPx),

	m_sampleWindowPx(),
	m_filter        (filter),
	m_merger        (makeNoOpMerger())
{
	calcSampleDimensions();

	PH_ASSERT(m_merger && getSampleWindowPx().isValid());
}

SamplingFilmBase::~SamplingFilmBase() = default;

void SamplingFilmBase::calcSampleDimensions()
{
	m_sampleWindowPx = TAABB2D<float64>(
		TVector2<float64>(getEffectiveWindowPx().minVertex).add(0.5).sub(m_filter.getHalfSizePx()),
		TVector2<float64>(getEffectiveWindowPx().maxVertex).sub(0.5).add(m_filter.getHalfSizePx()));

	if(!m_sampleWindowPx.isValid())
	{
		std::cerr << "warning: at TSamplingFilm::calcSampleDimensions(), "
		          << "invalid sampling window detected" << std::endl;
	}
}

SamplingFilmBase::Merger SamplingFilmBase::makeNoOpMerger()
{
	return []() -> void
	{
		std::cerr << "warning: at SamplingFilmBase::m_merger(), "
		          << "calling no-op merger" << std::endl;
	};
}

void SamplingFilmBase::setMerger(const Merger& merger)
{
	PH_ASSERT(merger);

	m_merger = merger;
}

void SamplingFilmBase::setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow)
{
	Film::setEffectiveWindowPx(effectiveWindow);

	calcSampleDimensions();
}

void SamplingFilmBase::mergeToParent() const
{
	m_merger();
}

TVector2<float64> SamplingFilmBase::getSampleResPx() const
{
	return {m_sampleWindowPx.getWidth(), m_sampleWindowPx.getHeight()};
}

const TAABB2D<float64>& SamplingFilmBase::getSampleWindowPx() const
{
	return m_sampleWindowPx;
}

// command interface

SamplingFilmBase::SamplingFilmBase(const InputPacket& packet) :

	Film(packet),

	m_sampleWindowPx(),
	m_filter        (SampleFilterFactory::createGaussianFilter()),
	m_merger        (makeNoOpMerger())
{
	const std::string filterName = packet.getString("filter-name", "box");

	if(filterName == "box")
	{
		m_filter = SampleFilterFactory::createBoxFilter();
	}
	else if(filterName == "gaussian")
	{
		m_filter = SampleFilterFactory::createGaussianFilter();
	}
	else if(filterName == "mn")
	{
		m_filter = SampleFilterFactory::createMNFilter();
	}

	PH_ASSERT(m_merger && m_sampleWindowPx.isValid());
}

SdlTypeInfo SamplingFilmBase::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "sampling-film-base");
}

void SamplingFilmBase::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph