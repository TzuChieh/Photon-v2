#include "Core/Filmic/HdrRgbFilm.h"
#include "Math/TVector3.h"
#include "PostProcess/Frame.h"
#include "FileIO/InputPacket.h"
#include "Core/Filmic/SampleFilter.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"

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
	const TVector2<float64> samplePosPx(xPx, yPx);

	// compute filter bounds
	TVector2<float64> filterMin(samplePosPx.sub(m_filter->getHalfSizePx()));
	TVector2<float64> filterMax(samplePosPx.add(m_filter->getHalfSizePx()));

	// reduce to effective bounds
	filterMin = filterMin.max(TVector2<float64>(m_effectiveWindowPx.minVertex));
	filterMax = filterMax.min(TVector2<float64>(m_effectiveWindowPx.maxVertex));

	// compute pixel index bounds (exclusive on x1y1)
	TVector2<int64> x0y0(filterMin.sub(0.5).ceil());
	TVector2<int64> x1y1(filterMax.sub(0.5).floor());
	x1y1.x += 1;
	x1y1.y += 1;

	for(int64 y = x0y0.y; y < x1y1.y; y++)
	{
		for(int64 x = x0y0.x; x < x1y1.x; x++)
		{
			const std::size_t baseIndex = y * static_cast<std::size_t>(m_effectiveResPx.x) + x;
			const float64 weight = m_filter->evaluate(x - (xPx - 0.5), y - (yPx - 0.5));

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

		if(m_effectiveResPx.x != subFilm->m_effectiveResPx.x || 
			m_effectiveResPx.y != subFilm->m_effectiveResPx.y)
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

	out_frame->resize(m_effectiveResPx.x, m_effectiveResPx.y);

	for(int64 y = 0; y < m_effectiveResPx.y; y++)
	{
		for(int64 x = 0; x < m_effectiveResPx.x; x++)
		{
			baseIndex = y * static_cast<std::size_t>(m_effectiveResPx.x) + x;

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

SdlTypeInfo HdrRgbFilm::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "hdr-rgb");
}

std::unique_ptr<HdrRgbFilm> HdrRgbFilm::ciLoad(const InputPacket& packet)
{
	const integer filmWidth    = packet.getInteger("width", 0, DataTreatment::REQUIRED());
	const integer filmHeight   = packet.getInteger("height", 0, DataTreatment::REQUIRED());
	const auto&   sampleFilter = std::make_shared<SampleFilter>(std::make_unique<TConstant2D<float64>>(1.0), 1, 1);

	return std::make_unique<HdrRgbFilm>(filmWidth, filmHeight, sampleFilter);
}

ExitStatus HdrRgbFilm::ciExecute(const std::shared_ptr<HdrRgbFilm>& targetResource,
                                 const std::string& functionName, 
                                 const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace