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
	m_widthPx(widthPx), m_heightPx(heightPx), 
	m_filter(filter),
	m_merger([]() -> void
	{
		std::cerr << "warning: at Film, " 
		          << "calling empty merger" << std::endl;
	})
{

}

Film::~Film() = default;

// command interface

Film::Film(const InputPacket& packet)
{
	const DataTreatment requiredDT(EDataImportance::REQUIRED, "Film requires pixel width and height");
	m_widthPx  = static_cast<uint32>(packet.getInteger("width", 0, requiredDT));
	m_heightPx = static_cast<uint32>(packet.getInteger("height", 0, requiredDT));

	// HACK
	auto filterFunc1 = std::make_unique<TConstant2D<float64>>(1.0);
	auto filterFunc2 = std::make_unique<TGaussian2D<float64>>(1.0, 1.0, 1.0);
	auto filterFunc3 = std::make_unique<TGaussian2D<float64>>(10.0, 10.0, 1.0);
	m_filter = std::make_shared<SampleFilter>(std::move(filterFunc1), 1, 1);
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