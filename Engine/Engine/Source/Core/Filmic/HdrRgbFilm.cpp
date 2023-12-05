#include "Core/Filmic/HdrRgbFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "Core/Filmic/SampleFilter.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilters.h"

#include <Common/assertion.h>

#include <cstddef>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <memory>

namespace ph
{

// OPT: precalculate resolutions (the ones end with ...ResPx)

HdrRgbFilm::HdrRgbFilm(
	const int64         actualWidthPx, 
	const int64         actualHeightPx,
	const SampleFilter& filter) : 

	HdrRgbFilm(
		actualWidthPx, 
		actualHeightPx,
		math::TAABB2D<int64>(
			math::TVector2<int64>(0, 0),
			math::TVector2<int64>(actualWidthPx, actualHeightPx)),
		filter)
{}

HdrRgbFilm::HdrRgbFilm(
	const int64                 actualWidthPx, 
	const int64                 actualHeightPx,
	const math::TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter&         filter) :

	TSamplingFilm<math::Spectrum>(
		actualWidthPx, 
		actualHeightPx, 
		effectiveWindowPx, 
		filter),

	m_pixelRadianceSensors()
{
	resizeRadianceSensorBuffer();
	clear();
}

HdrRgbFilm::HdrRgbFilm(HdrRgbFilm&& other) :

	TSamplingFilm<math::Spectrum>(std::move(other)),

	m_pixelRadianceSensors(std::move(other.m_pixelRadianceSensors))
{}

void HdrRgbFilm::addSample(
	const float64         xPx, 
	const float64         yPx, 
	const math::Spectrum& radiance)
{
	PH_ASSERT_MSG(radiance.isFinite(), radiance.toString());

	const auto rgb = math::Vector3R(radiance.toLinearSRGB(math::EColorUsage::EMR));
	addSample(xPx, yPx, rgb);
}

void HdrRgbFilm::addSample(
	const float64         xPx, 
	const float64         yPx, 
	const math::Vector3R& rgb)
{
	const math::TVector2<float64> samplePosPx(xPx, yPx);

	// compute filter bounds
	math::TVector2<float64> filterMin(samplePosPx.sub(getFilter().getHalfSizePx()));
	math::TVector2<float64> filterMax(samplePosPx.add(getFilter().getHalfSizePx()));

	// reduce to effective bounds
	filterMin = filterMin.max(math::TVector2<float64>(getEffectiveWindowPx().getMinVertex()));
	filterMax = filterMax.min(math::TVector2<float64>(getEffectiveWindowPx().getMaxVertex()));

	// compute pixel index bounds (exclusive on x1y1)
	math::TVector2<int64> x0y0(filterMin.sub(0.5).ceil());
	math::TVector2<int64> x1y1(filterMax.sub(0.5).floor());
	x1y1.x() += 1;
	x1y1.y() += 1;

	for(int64 y = x0y0.y(); y < x1y1.y(); y++)
	{
		for(int64 x = x0y0.x(); x < x1y1.x(); x++)
		{
			// TODO: factor out the -0.5 part
			const float64 filterX = x - (xPx - 0.5);
			const float64 filterY = y - (yPx - 0.5);

			const std::size_t fx = x - getEffectiveWindowPx().getMinVertex().x();
			const std::size_t fy = y - getEffectiveWindowPx().getMinVertex().y();
			const std::size_t index = fy * static_cast<std::size_t>(getEffectiveResPx().x()) + fx;
			
			const float64 weight = getFilter().evaluate(filterX, filterY);

			m_pixelRadianceSensors[index].accuR      += rgb.r() * weight;
			m_pixelRadianceSensors[index].accuG      += rgb.g() * weight;
			m_pixelRadianceSensors[index].accuB      += rgb.b() * weight;
			m_pixelRadianceSensors[index].accuWeight += weight;
		}
	}
}

//void HdrRgbFilm::genChild(
//	const TAABB2D<int64>&                        effectiveWindowPx,
//	TMergeableFilmProxy<Spectrum>* const out_film)
//{
//	auto childFilm = std::make_unique<HdrRgbFilm>(
//		getActualResPx().x, 
//		getActualResPx().y,
//		effectiveWindowPx, 
//		getFilter());
//
//	HdrRgbFilm* parent = this;
//	HdrRgbFilm* child  = childFilm.getReference();
//
//	*out_film = TMergeableFilmProxy<Spectrum>(
//		child, 
//		[=]()
//		{
//			PH_ASSERT(parent);
//			PH_ASSERT(child);
//
//			parent->mergeWith(*child);
//		});
//}

void HdrRgbFilm::developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const
{
	if(out_frame.widthPx()  != getActualResPx().x() ||
	   out_frame.heightPx() != getActualResPx().y())
	{
		std::cerr << "warning: at HdrRgbFilm::develop(), "
		          << "input frame dimension mismatch" << std::endl;
		return;
	}

	math::TAABB2D<int64> frameIndexBound(getEffectiveWindowPx());
	frameIndexBound.intersectWith(regionPx);
	frameIndexBound.setMaxVertex(frameIndexBound.getMaxVertex().sub(1));

	float64     sensorR, sensorG, sensorB;
	float64     reciWeight;
	std::size_t fx, fy, filmIndex;

	// FIXME: we should iterate in frameIndexBound only
	for(int64 y = 0; y < getActualResPx().y(); y++)
	{
		for(int64 x = 0; x < getActualResPx().x(); x++)
		{
			if(!frameIndexBound.isIntersectingArea({x, y}))
			{
				continue;
			}

			fx = x - getEffectiveWindowPx().getMinVertex().x();
			fy = y - getEffectiveWindowPx().getMinVertex().y();
			filmIndex = fy * static_cast<std::size_t>(getEffectiveResPx().x()) + fx;

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
	math::TAABB2D<int64> validRegion(this->getEffectiveWindowPx());
	validRegion.intersectWith(other.getEffectiveWindowPx());

	for(int64 y = validRegion.getMinVertex().y(); y < validRegion.getMaxVertex().y(); ++y)
	{
		const std::size_t thisY = y - this->getEffectiveWindowPx().getMinVertex().y();
		const std::size_t otherY = y - other.getEffectiveWindowPx().getMinVertex().y();
		const std::size_t thisBaseIndex = thisY * static_cast<std::size_t>(this->getEffectiveResPx().x());
		const std::size_t otherBaseIndex = otherY * static_cast<std::size_t>(other.getEffectiveResPx().x());

		for(int64 x = validRegion.getMinVertex().x(); x < validRegion.getMaxVertex().x(); ++x)
		{
			const std::size_t thisX = x - this->getEffectiveWindowPx().getMinVertex().x();
			const std::size_t otherX = x - other.getEffectiveWindowPx().getMinVertex().x();
			const std::size_t thisI = thisBaseIndex + thisX;
			const std::size_t otherI = otherBaseIndex + otherX;

			m_pixelRadianceSensors[thisI].accuR      += other.m_pixelRadianceSensors[otherI].accuR;
			m_pixelRadianceSensors[thisI].accuG      += other.m_pixelRadianceSensors[otherI].accuG;
			m_pixelRadianceSensors[thisI].accuB      += other.m_pixelRadianceSensors[otherI].accuB;
			m_pixelRadianceSensors[thisI].accuWeight += other.m_pixelRadianceSensors[otherI].accuWeight;
		}
	}
}

void HdrRgbFilm::setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow)
{
	TSamplingFilm<math::Spectrum>::setEffectiveWindowPx(effectiveWindow);

	resizeRadianceSensorBuffer();
	clear();
}

HdrRgbFilm& HdrRgbFilm::operator = (HdrRgbFilm&& other)
{
	TSamplingFilm<math::Spectrum>::operator = (std::move(other));

	m_pixelRadianceSensors = std::move(other.m_pixelRadianceSensors);

	return *this;
}

void HdrRgbFilm::resizeRadianceSensorBuffer()
{
	m_pixelRadianceSensors.resize(getEffectiveWindowPx().getArea());
}

void HdrRgbFilm::setPixel(
	const float64         xPx, 
	const float64         yPx, 
	const math::Spectrum& spectrum)
{
	const std::size_t filmX = std::min(static_cast<std::size_t>(xPx), static_cast<std::size_t>(getActualResPx().x()) - 1);
	const std::size_t filmY = std::min(static_cast<std::size_t>(yPx), static_cast<std::size_t>(getActualResPx().y()) - 1);

	const std::size_t ix = filmX - getEffectiveWindowPx().getMinVertex().x();
	const std::size_t iy = filmY - getEffectiveWindowPx().getMinVertex().y();
	const std::size_t index = iy * static_cast<std::size_t>(getEffectiveResPx().x()) + ix;

	const auto rgb = math::Vector3R(spectrum.toLinearSRGB(math::EColorUsage::EMR));

	m_pixelRadianceSensors[index].accuR      = rgb.r();
	m_pixelRadianceSensors[index].accuG      = rgb.g();
	m_pixelRadianceSensors[index].accuB      = rgb.b();
	m_pixelRadianceSensors[index].accuWeight = 1.0;
}

}// end namespace
