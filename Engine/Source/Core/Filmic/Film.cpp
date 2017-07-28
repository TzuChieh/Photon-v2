#include "Core/Filmic/Film.h"
#include "Math/TVector3.h"
#include "PostProcess/Frame.h"
#include "FileIO/InputPacket.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilter.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

Film::Film(const uint64 actualWidthPx, const uint64 actualHeightPx,
           const std::shared_ptr<SampleFilter>& filter) :
	Film(actualWidthPx, actualHeightPx,
	     TAABB2D<uint64>(TVector2<uint64>(0, 0),
	                     TVector2<uint64>(actualWidthPx, actualHeightPx)),
	     filter)
{

}

Film::Film(const uint64 actualWidthPx, const uint64 actualHeightPx,
           const TAABB2D<uint64>& effectiveWindowPx,
           const std::shared_ptr<SampleFilter>& filter) : 
	m_actualResPx(static_cast<int64>(actualWidthPx), 
	              static_cast<int64>(actualHeightPx)), 
	m_effectiveResPx(static_cast<int64>(effectiveWindowPx.getWidth()), 
	                 static_cast<int64>(effectiveWindowPx.getHeight())),
	m_sampleResPx(static_cast<float64>(effectiveWindowPx.getWidth()) - 1.0 + filter->getSizePx().x, 
	              static_cast<float64>(effectiveWindowPx.getHeight()) - 1.0 + filter->getSizePx().y),
	m_effectiveWindowPx(effectiveWindowPx),
	m_sampleWindowPx(TVector2<float64>(effectiveWindowPx.minVertex).add(0.5).sub(filter->getHalfSizePx()),
	                 TVector2<float64>(effectiveWindowPx.maxVertex).sub(0.5).add(filter->getHalfSizePx())),
	m_filter(filter),
	m_merger([]() -> void
	{
		std::cerr << "warning: at Film::m_merger(), " 
		          << "calling empty merger" << std::endl;
	})
{
	if(!m_effectiveWindowPx.isValid() || !m_sampleWindowPx.isValid())
	{
		std::cerr << "warning: at Film::Film(), "
		          << "invalid window calculated" << std::endl;
	}
}

Film::~Film() = default;

// command interface

SdlTypeInfo Film::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "film");
}

ExitStatus Film::ciExecute(const std::shared_ptr<Film>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph