#include "Core/Filmic/TSamplingFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "FileIO/SDL/InputPacket.h"
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
	const int64 actualWidthPx, const int64 actualHeightPx,
	const SampleFilter& filter) :

	TSamplingFilm(
		actualWidthPx, actualHeightPx,
		TAABB2D<int64>(TVector2<int64>(0, 0),
		               TVector2<int64>(actualWidthPx, actualHeightPx)),
		filter)
{}

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(
	const int64 actualWidthPx, const int64 actualHeightPx,
	const TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter& filter) : 

	Film(actualWidthPx, actualHeightPx, effectiveWindowPx),

	m_sampleResPx   (),
	m_sampleWindowPx(),
	m_filter        (filter),
	m_merger        (makeDefaultMerger())
{
	PH_ASSERT(m_merger);

	calcSampleDimensions();
}

template<typename Sample>
inline TSamplingFilm<Sample>::~TSamplingFilm() = default;

template<typename Sample>
inline void TSamplingFilm<Sample>::calcSampleDimensions()
{
	m_sampleResPx = TVector2<float64>(
		m_effectiveWindowPx.getWidth()  - 1.0 + m_filter.getSizePx().x,
		m_effectiveWindowPx.getHeight() - 1.0 + m_filter.getSizePx().y);

	m_sampleWindowPx = TAABB2D<float64>(
		TVector2<float64>(m_effectiveWindowPx.minVertex).add(0.5).sub(m_filter.getHalfSizePx()),
		TVector2<float64>(m_effectiveWindowPx.maxVertex).sub(0.5).add(m_filter.getHalfSizePx()));

	if(!m_sampleWindowPx.isValid())
	{
		std::cerr << "warning: at TSamplingFilm::calcSampleDimensions(), "
		          << "invalid sampling window detected" << std::endl;
	}
}

template<typename Sample>
std::function<void()> TSamplingFilm<Sample>::makeDefaultMerger()
{
	return []() -> void
	{
		std::cerr << "warning: at TSamplingFilm::m_merger(), "
		          << "calling empty merger" << std::endl;
	};
}

// command interface

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(const InputPacket& packet) : 

	Film(packet),

	m_sampleResPx   (),
	m_sampleWindowPx(),
	m_filter        (SampleFilterFactory::createGaussianFilter()),
	m_merger        (makeDefaultMerger())
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

	PH_ASSERT(m_merger);
}

template<typename Sample>
inline SdlTypeInfo TSamplingFilm<Sample>::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "sampling-film");
}

template<typename Sample>
inline void TSamplingFilm<Sample>::ciRegister(CommandRegister& cmdRegister) 
{}

}// end namespace ph