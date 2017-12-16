#include "Core/Filmic/HdrRgbFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "FileIO/InputPacket.h"
#include "Core/Filmic/SampleFilter.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilterFactory.h"

#include <cstddef>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace ph
{

HdrRgbFilm::HdrRgbFilm(const int64 actualWidthPx, const int64 actualHeightPx,
                       const std::shared_ptr<SampleFilter>& filter) : 
	HdrRgbFilm(actualWidthPx, actualHeightPx,
	           TAABB2D<int64>(TVector2<int64>(0, 0),
	                          TVector2<int64>(actualWidthPx, actualHeightPx)),
	           filter)
{

}

HdrRgbFilm::HdrRgbFilm(const int64 actualWidthPx, const int64 actualHeightPx,
                       const TAABB2D<int64>& effectiveWindowPx,
                       const std::shared_ptr<SampleFilter>& filter) :
	Film(actualWidthPx, actualHeightPx, effectiveWindowPx, filter),
	m_pixelRadianceSensors(effectiveWindowPx.calcArea(), RadianceSensor())
{

}

HdrRgbFilm::~HdrRgbFilm() = default;

void HdrRgbFilm::addSample(const float64 xPx, const float64 yPx, const SpectralStrength& radiance)
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
			// TODO: factor out -0.5 part
			const float64 filterX = x - (xPx - 0.5);
			const float64 filterY = y - (yPx - 0.5);

			const std::size_t fx = x - m_effectiveWindowPx.minVertex.x;
			const std::size_t fy = y - m_effectiveWindowPx.minVertex.y;
			const std::size_t index = fy * static_cast<std::size_t>(m_effectiveResPx.x) + fx;

			
			const float64   weight = m_filter->evaluate(filterX, filterY);
			const Vector3R& rgb    = radiance.genRgb();

			m_pixelRadianceSensors[index].accuR      += rgb.x * weight;
			m_pixelRadianceSensors[index].accuG      += rgb.y * weight;
			m_pixelRadianceSensors[index].accuB      += rgb.z * weight;
			m_pixelRadianceSensors[index].accuWeight += weight;
		}
	}
}

std::unique_ptr<Film> HdrRgbFilm::genChild(const TAABB2D<int64>& effectiveWindowPx)
{
	auto childFilm = std::make_unique<HdrRgbFilm>(m_actualResPx.x, m_actualResPx.y, 
	                                              effectiveWindowPx, 
	                                              m_filter);
	HdrRgbFilm* parent = this;
	HdrRgbFilm* child  = childFilm.get();
	childFilm->m_merger = [=]() -> void
	{
		parent->mergeWith(*child);
	};

	return std::move(childFilm);
}

void HdrRgbFilm::developRegion(TFrame<real>& out_frame, const TAABB2D<int64>& regionPx) const
{
	if(out_frame.widthPx()  != m_actualResPx.x ||
	   out_frame.heightPx() != m_actualResPx.y)
	{
		std::cerr << "warning: at HdrRgbFilm::develop(), "
		          << "input frame dimension mismatch" << std::endl;
		return;
	}

	TAABB2D<int64> frameIndexBound(m_effectiveWindowPx);
	frameIndexBound.intersectWith(regionPx);
	frameIndexBound.maxVertex.subLocal(1);

	float64     sensorR, sensorG, sensorB;
	float64     reciWeight;
	std::size_t fx, fy, filmIndex;

	for(int64 y = 0; y < m_actualResPx.y; y++)
	{
		for(int64 x = 0; x < m_actualResPx.x; x++)
		{
			if(!frameIndexBound.isIntersectingArea({x, y}))
			{
				continue;
			}

			fx = x - m_effectiveWindowPx.minVertex.x;
			fy = y - m_effectiveWindowPx.minVertex.y;
			filmIndex = fy * static_cast<std::size_t>(m_effectiveResPx.x) + fx;

			const float64 sensorWeight = m_pixelRadianceSensors[filmIndex].accuWeight;

			// prevent division by zero
			reciWeight = sensorWeight == 0.0 ? 0.0 : 1.0 / sensorWeight;

			sensorR = m_pixelRadianceSensors[filmIndex].accuR * reciWeight;
			sensorG = m_pixelRadianceSensors[filmIndex].accuG * reciWeight;
			sensorB = m_pixelRadianceSensors[filmIndex].accuB * reciWeight;

			// TODO: prevent negative pixel
			out_frame.setPixel(static_cast<uint32>(x), static_cast<uint32>(y),
			                   Vector3R(TVector3<float64>(sensorR, sensorG, sensorB)));
		}
	}
}

void HdrRgbFilm::clear()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), RadianceSensor());
}

void HdrRgbFilm::mergeWith(const HdrRgbFilm& other)
{
	const std::size_t numSensors = other.m_pixelRadianceSensors.size();
	for(std::size_t i = 0; i < numSensors; i++)
	{
		m_pixelRadianceSensors[i].accuR      += other.m_pixelRadianceSensors[i].accuR;
		m_pixelRadianceSensors[i].accuG      += other.m_pixelRadianceSensors[i].accuG;
		m_pixelRadianceSensors[i].accuB      += other.m_pixelRadianceSensors[i].accuB;
		m_pixelRadianceSensors[i].accuWeight += other.m_pixelRadianceSensors[i].accuWeight;
	}
}

// command interface

SdlTypeInfo HdrRgbFilm::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "hdr-rgb");
}

void HdrRgbFilm::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<HdrRgbFilm>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<HdrRgbFilm> HdrRgbFilm::ciLoad(const InputPacket& packet)
{
	const integer     filmWidth  = packet.getInteger("width",  0, DataTreatment::REQUIRED());
	const integer     filmHeight = packet.getInteger("height", 0, DataTreatment::REQUIRED());
	const std::string filterName = packet.getString("filter-name", "box");
	const integer     rectX      = packet.getInteger("rect-x", 0);
	const integer     rectY      = packet.getInteger("rect-y", 0);
	const integer     rectW      = packet.getInteger("rect-w", filmWidth);
	const integer     rectH      = packet.getInteger("rect-h", filmHeight);

	std::shared_ptr<SampleFilter> sampleFilter;
	if(filterName == "box")
	{
		sampleFilter = std::make_shared<SampleFilter>(SampleFilterFactory::createBoxFilter());
	}
	else if(filterName == "gaussian")
	{
		sampleFilter = std::make_shared<SampleFilter>(SampleFilterFactory::createGaussianFilter());
	}
	else if(filterName == "mn")
	{
		sampleFilter = std::make_shared<SampleFilter>(SampleFilterFactory::createMNFilter());
	}
	else
	{
		std::cerr << "warning: at HdrRgbFilm::ciLoad(), " 
		          << "unknown filter name specified: " << filterName << std::endl;
	}

	const TAABB2D<int64> effectWindowPx({rectX, rectY}, 
	                                    {rectX + rectW, rectY + rectH});
	return std::make_unique<HdrRgbFilm>(filmWidth, filmHeight, effectWindowPx, sampleFilter);
}

}// end namespace