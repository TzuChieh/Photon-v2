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

Film::Film(const uint32 widthPx, const uint32 heightPx, 
           const std::shared_ptr<SampleFilter>& filter) :
	Film(widthPx, heightPx, 
	     TAABB2D<int64>(TVector2<int64>(0, 0), 
	                    TVector2<int64>(widthPx, heightPx)),
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

Film::~Film() = default;

// command interface

// HACK
Film::Film(const InputPacket& packet) : 
	Film(packet.getInteger("width", 0, DataTreatment::REQUIRED()), packet.getInteger("height", 0, DataTreatment::REQUIRED()), 
		std::make_shared<SampleFilter>(std::make_unique<TConstant2D<float64>>(1.0), 1, 1))
{
	//const DataTreatment requiredDT(EDataImportance::REQUIRED, "Film requires pixel width and height");
	//m_widthPx  = static_cast<uint32>(packet.getInteger("width",  0, requiredDT));
	//m_heightPx = static_cast<uint32>(packet.getInteger("height", 0, requiredDT));

	// HACK
	//auto filterFunc1 = std::make_unique<TConstant2D<float64>>(1.0);
	//auto filterFunc2 = std::make_unique<TGaussian2D<float64>>(1.0, 1.0, 1.0);
	//auto filterFunc3 = std::make_unique<TGaussian2D<float64>>(10.0, 10.0, 1.0);
	//m_filter = std::make_shared<SampleFilter>(std::move(filterFunc1), 1, 1);
	//m_filter = std::make_shared<SampleFilter>(std::move(filterFunc2), 2, 2);
	//m_filter = std::make_shared<SampleFilter>(std::move(filterFunc3), 16, 16);
}

SdlTypeInfo Film::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "film");
}

ExitStatus Film::ciExecute(const std::shared_ptr<Film>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph