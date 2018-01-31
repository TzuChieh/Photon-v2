#include "Core/Filmic/Film.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "FileIO/InputPacket.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilter.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

Film::Film(const int64 actualWidthPx, const int64 actualHeightPx,
           const std::shared_ptr<SampleFilter>& filter) :
	Film(actualWidthPx, actualHeightPx,
	     TAABB2D<int64>(TVector2<int64>(0, 0),
	                    TVector2<int64>(actualWidthPx, actualHeightPx)),
	     filter)
{

}

Film::Film(const int64 actualWidthPx, const int64 actualHeightPx,
           const TAABB2D<int64>& effectiveWindowPx,
           const std::shared_ptr<SampleFilter>& filter) : 
	m_actualResPx(actualWidthPx, actualHeightPx), 
	m_effectiveResPx(effectiveWindowPx.getWidth(), effectiveWindowPx.getHeight()),
	m_sampleResPx(effectiveWindowPx.getWidth() - 1.0 + filter->getSizePx().x, 
	              effectiveWindowPx.getHeight() - 1.0 + filter->getSizePx().y),
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

void Film::develop(HdrRgbFrame& out_frame) const
{
	developRegion(out_frame, m_effectiveWindowPx);
}

void Film::develop(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const
{
	developRegion(out_frame, regionPx);
}

Film::~Film() = default;

// command interface

SdlTypeInfo Film::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "film");
}

void Film::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph