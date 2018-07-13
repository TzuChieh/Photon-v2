#include "Core/Filmic/HdrRgbFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/Filmic/SampleFilter.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "Common/assertion.h"

#include <cstddef>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <memory>

namespace ph
{

// OPT: precalculate resolutions (the ones end with ...ResPx)

HdrRgbFilm::HdrRgbFilm(
	const int64 actualWidthPx, const int64 actualHeightPx,
	const SampleFilter& filter) : 

	HdrRgbFilm(
		actualWidthPx, actualHeightPx,
		TAABB2D<int64>(TVector2<int64>(0, 0),
		               TVector2<int64>(actualWidthPx, actualHeightPx)),
		filter)
{}

HdrRgbFilm::HdrRgbFilm(
	const int64 actualWidthPx, const int64 actualHeightPx,
	const TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter& filter) :

	SpectralSamplingFilm(
		actualWidthPx, actualHeightPx, 
		effectiveWindowPx, 
		filter),

	m_pixelRadianceSensors()
{
	resizeRadianceSensorBuffer();
	clear();
}

HdrRgbFilm::~HdrRgbFilm() = default;

void HdrRgbFilm::addSample(
	const float64 xPx, const float64 yPx, 
	const SpectralStrength& radiance)
{
	const Vector3R rgb = radiance.genLinearSrgb(EQuantity::EMR);
	addSample(xPx, yPx, rgb);
}

void HdrRgbFilm::addSample(const float64 xPx, const float64 yPx, const Vector3R& rgb)
{
	const TVector2<float64> samplePosPx(xPx, yPx);

	// compute filter bounds
	TVector2<float64> filterMin(samplePosPx.sub(m_filter.getHalfSizePx()));
	TVector2<float64> filterMax(samplePosPx.add(m_filter.getHalfSizePx()));

	// reduce to effective bounds
	filterMin = filterMin.max(TVector2<float64>(getEffectiveWindowPx().minVertex));
	filterMax = filterMax.min(TVector2<float64>(getEffectiveWindowPx().maxVertex));

	// compute pixel index bounds (exclusive on x1y1)
	TVector2<int64> x0y0(filterMin.sub(0.5).ceil());
	TVector2<int64> x1y1(filterMax.sub(0.5).floor());
	x1y1.x += 1;
	x1y1.y += 1;

	for(int64 y = x0y0.y; y < x1y1.y; y++)
	{
		for(int64 x = x0y0.x; x < x1y1.x; x++)
		{
			// TODO: factor out the -0.5 part
			const float64 filterX = x - (xPx - 0.5);
			const float64 filterY = y - (yPx - 0.5);

			const std::size_t fx = x - getEffectiveWindowPx().minVertex.x;
			const std::size_t fy = y - getEffectiveWindowPx().minVertex.y;
			const std::size_t index = fy * static_cast<std::size_t>(getEffectiveResPx().x) + fx;
			
			const float64 weight = m_filter.evaluate(filterX, filterY);

			m_pixelRadianceSensors[index].accuR      += rgb.x * weight;
			m_pixelRadianceSensors[index].accuG      += rgb.y * weight;
			m_pixelRadianceSensors[index].accuB      += rgb.z * weight;
			m_pixelRadianceSensors[index].accuWeight += weight;
		}
	}
}

std::unique_ptr<SpectralSamplingFilm> HdrRgbFilm::genSamplingChild(const TAABB2D<int64>& effectiveWindowPx)
{
	auto childFilm = std::make_unique<HdrRgbFilm>(getActualResPx().x, getActualResPx().y,
	                                              effectiveWindowPx, 
	                                              m_filter);
	HdrRgbFilm* parent = this;
	HdrRgbFilm* child  = childFilm.get();
	childFilm->setMerger([=]() -> void
	{
		PH_ASSERT(parent != nullptr && child != nullptr);

		parent->mergeWith(*child);
	});

	return std::move(childFilm);
}

void HdrRgbFilm::developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const
{
	if(out_frame.widthPx()  != getActualResPx().x ||
	   out_frame.heightPx() != getActualResPx().y)
	{
		std::cerr << "warning: at HdrRgbFilm::develop(), "
		          << "input frame dimension mismatch" << std::endl;
		return;
	}

	TAABB2D<int64> frameIndexBound(getEffectiveWindowPx());
	frameIndexBound.intersectWith(regionPx);
	frameIndexBound.maxVertex.subLocal(1);

	float64     sensorR, sensorG, sensorB;
	float64     reciWeight;
	std::size_t fx, fy, filmIndex;

	for(int64 y = 0; y < getActualResPx().y; y++)
	{
		for(int64 x = 0; x < getActualResPx().x; x++)
		{
			if(!frameIndexBound.isIntersectingArea({x, y}))
			{
				continue;
			}

			fx = x - getEffectiveWindowPx().minVertex.x;
			fy = y - getEffectiveWindowPx().minVertex.y;
			filmIndex = fy * static_cast<std::size_t>(getEffectiveResPx().x) + fx;

			const float64 sensorWeight = m_pixelRadianceSensors[filmIndex].accuWeight;

			// prevent division by zero
			reciWeight = sensorWeight == 0.0 ? 0.0 : 1.0 / sensorWeight;

			sensorR = m_pixelRadianceSensors[filmIndex].accuR * reciWeight;
			sensorG = m_pixelRadianceSensors[filmIndex].accuG * reciWeight;
			sensorB = m_pixelRadianceSensors[filmIndex].accuB * reciWeight;

			// TODO: prevent negative pixel
			out_frame.setPixel(
				static_cast<uint32>(x), static_cast<uint32>(y),
				HdrRgbFrame::Pixel({static_cast<HdrComponent>(sensorR), 
			                        static_cast<HdrComponent>(sensorG), 
			                        static_cast<HdrComponent>(sensorB)}));
		}
	}
}

void HdrRgbFilm::clear()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), RadianceSensor());
}

void HdrRgbFilm::mergeWith(const HdrRgbFilm& other)
{
	// FIXME: this is wrong, must respect effective window

	const std::size_t numSensors = other.m_pixelRadianceSensors.size();
	for(std::size_t i = 0; i < numSensors; i++)
	{
		m_pixelRadianceSensors[i].accuR      += other.m_pixelRadianceSensors[i].accuR;
		m_pixelRadianceSensors[i].accuG      += other.m_pixelRadianceSensors[i].accuG;
		m_pixelRadianceSensors[i].accuB      += other.m_pixelRadianceSensors[i].accuB;
		m_pixelRadianceSensors[i].accuWeight += other.m_pixelRadianceSensors[i].accuWeight;
	}
}

void HdrRgbFilm::setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow)
{
	SpectralSamplingFilm::setEffectiveWindowPx(effectiveWindow);

	resizeRadianceSensorBuffer();
	clear();
}

void HdrRgbFilm::resizeRadianceSensorBuffer()
{
	m_pixelRadianceSensors.resize(getEffectiveWindowPx().calcArea());
}

// command interface

HdrRgbFilm::HdrRgbFilm(const InputPacket& packet) : 
	SpectralSamplingFilm(packet),
	m_pixelRadianceSensors(getEffectiveWindowPx().calcArea(), RadianceSensor())
{
	PH_ASSERT(!m_pixelRadianceSensors.empty());
}

SdlTypeInfo HdrRgbFilm::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "hdr-rgb");
}

void HdrRgbFilm::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<HdrRgbFilm>([](const InputPacket& packet)
	{
		return std::make_unique<HdrRgbFilm>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace