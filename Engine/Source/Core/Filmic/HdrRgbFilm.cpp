#include "Core/Filmic/HdrRgbFilm.h"
#include "Math/TVector3.h"
#include "PostProcess/Frame.h"
#include "FileIO/InputPacket.h"
#include "Core/Filmic/SampleFilter.h"

#include <cstddef>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace ph
{

HdrRgbFilm::HdrRgbFilm(uint32 widthPx, uint32 heightPx,
                       const std::shared_ptr<SampleFilter>& filter) : 
	Film(widthPx, heightPx, filter),
	m_pixelRadianceSensors(static_cast<std::size_t>(widthPx) * static_cast<std::size_t>(heightPx), RadianceSensor())
{

}

HdrRgbFilm::~HdrRgbFilm() = default;

void HdrRgbFilm::addSample(const float64 xPx, const float64 yPx, const Vector3R& radiance)
{
	const float64 discreteXpx = xPx - 0.5;
	const float64 discreteYpx = yPx - 0.5;
	int32 filterX0 = static_cast<int32>(std::ceil(discreteXpx - m_filter->getHalfWidthPx()));
	int32 filterX1 = static_cast<int32>(std::floor(discreteXpx + m_filter->getHalfWidthPx())) + 1;
	int32 filterY0 = static_cast<int32>(std::ceil(discreteYpx - m_filter->getHalfHeightPx()));
	int32 filterY1 = static_cast<int32>(std::floor(discreteYpx + m_filter->getHalfHeightPx())) + 1;
	filterX0 = filterX0 < 0 ? 0 : filterX0;
	filterX1 = filterX1 > static_cast<int32>(m_widthPx) ? static_cast<int32>(m_widthPx) : filterX1;
	filterY0 = filterY0 < 0 ? 0 : filterY0;
	filterY1 = filterY1 > static_cast<int32>(m_heightPx) ? static_cast<int32>(m_heightPx) : filterY1;

	for(int y = filterY0; y < filterY1; y++)
	{
		for(int x = filterX0; x < filterX1; x++)
		{
			const std::size_t baseIndex = y * static_cast<std::size_t>(m_widthPx) + x;
			const float64 weight = m_filter->evaluate(x - discreteXpx, y - discreteYpx);

			m_pixelRadianceSensors[baseIndex].accuR += static_cast<float64>(radiance.x) * weight;
			m_pixelRadianceSensors[baseIndex].accuG += static_cast<float64>(radiance.y) * weight;
			m_pixelRadianceSensors[baseIndex].accuB += static_cast<float64>(radiance.z) * weight;
			m_pixelRadianceSensors[baseIndex].accuWeight += weight;
		}
	}
}

std::unique_ptr<Film> HdrRgbFilm::genChild(uint32 widthPx, uint32 heightPx)
{
	auto subFilm = std::make_unique<HdrRgbFilm>(widthPx, heightPx, m_filter);
	subFilm->m_merger = [&, this, subFilm = subFilm.get()]() -> void
	{
		// HACK

		if(m_widthPx != subFilm->m_widthPx || m_heightPx != subFilm->m_heightPx)
		{
			std::cerr << "warning: at Film::accumulateRadiance(), film dimensions mismatch" << std::endl;
			return;
		}

		const std::size_t numSensors = subFilm->m_pixelRadianceSensors.size();
		for(std::size_t i = 0; i < numSensors; i++)
		{
			m_pixelRadianceSensors[i].accuR += subFilm->m_pixelRadianceSensors[i].accuR;
			m_pixelRadianceSensors[i].accuG += subFilm->m_pixelRadianceSensors[i].accuG;
			m_pixelRadianceSensors[i].accuB += subFilm->m_pixelRadianceSensors[i].accuB;
			m_pixelRadianceSensors[i].accuWeight += subFilm->m_pixelRadianceSensors[i].accuWeight;
		}
	};

	return std::move(subFilm);
}

void HdrRgbFilm::develop(Frame* const out_frame) const
{
	float64 sensorR;
	float64 sensorG;
	float64 sensorB;
	float64 reciSenseCount;
	std::size_t baseIndex;

	out_frame->resize(m_widthPx, m_heightPx);

	for(uint32 y = 0; y < m_heightPx; y++)
	{
		for(uint32 x = 0; x < m_widthPx; x++)
		{
			baseIndex = y * static_cast<std::size_t>(m_widthPx) + x;

			sensorR = m_pixelRadianceSensors[baseIndex].accuR;
			sensorG = m_pixelRadianceSensors[baseIndex].accuG;
			sensorB = m_pixelRadianceSensors[baseIndex].accuB;
			const float64 senseWeight = static_cast<float64>(m_pixelRadianceSensors[baseIndex].accuWeight);

			// to prevent divide by zero
			reciSenseCount = senseWeight == 0.0 ? 0.0 : 1.0 / senseWeight;

			sensorR *= reciSenseCount;
			sensorG *= reciSenseCount;
			sensorB *= reciSenseCount;

			out_frame->setPixel(x, y, static_cast<real>(sensorR), static_cast<real>(sensorG), static_cast<real>(sensorB));
		}
	}
}

void HdrRgbFilm::clear()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), RadianceSensor());
}

// command interface

HdrRgbFilm::HdrRgbFilm(const InputPacket& packet) : 
	Film(packet)
{
	//const DataTreatment requiredDT(EDataImportance::REQUIRED, "Film requires pixel width and height");
	//m_widthPx = static_cast<uint32>(packet.getInteger("width", 0, requiredDT));
	//m_heightPx = static_cast<uint32>(packet.getInteger("height", 0, requiredDT));

	const std::size_t numSensors = static_cast<std::size_t>(m_widthPx) * static_cast<std::size_t>(m_heightPx);
	m_pixelRadianceSensors = std::vector<RadianceSensor>(numSensors, RadianceSensor());
}

SdlTypeInfo HdrRgbFilm::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "hdr-rgb");
}

ExitStatus HdrRgbFilm::ciExecute(const std::shared_ptr<HdrRgbFilm>& targetResource,
                                 const std::string& functionName, 
                                 const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace