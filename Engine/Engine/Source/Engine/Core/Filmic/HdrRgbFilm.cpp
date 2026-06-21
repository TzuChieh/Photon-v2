#include "Engine/Core/Filmic/HdrRgbFilm.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Frame/TFrame.h"
#include "Engine/Core/Filmic/SampleFilter.h"
#include "Engine/Math/Function/TConstant2D.h"
#include "Engine/Math/Function/TGaussian2D.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cstddef>
#include <cmath>

namespace ph
{

namespace
{

inline math::Vector3R spectrum_sample_to_rgb(const math::Spectrum& sample)
{
	// Currently this assumes the incoming sample is energy
	return math::Vector3R(sample.toLinearSRGB(math::EColorUsage::EMR));
}

}// end anonymous namespace

// OPT: precalculate resolutions (the ones end with ...ResPx)

HdrRgbFilm::HdrRgbFilm(
	const int64                 actualWidthPx, 
	const int64                 actualHeightPx,
	const SampleFilter&         filter)

	: HdrRgbFilm(
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
	const SampleFilter&         filter)

	: TSamplingFilm<math::Spectrum>(
		actualWidthPx, 
		actualHeightPx, 
		effectiveWindowPx, 
		filter)

	, m_pixelRadianceSensors()
{
	resizeRadianceSensorBuffer();
	clearRadianceSensors();
}

void HdrRgbFilm::addSample(
	const float64         xPx, 
	const float64         yPx, 
	const math::Spectrum& sample)
{
	PH_ASSERT_MSG(sample.isFinite(), sample.toString());

	addRgbSample(xPx, yPx, spectrum_sample_to_rgb(sample));
}

void HdrRgbFilm::setPixel(
	const float64         xPx, 
	const float64         yPx, 
	const math::Spectrum& sample)
{
	PH_ASSERT_MSG(sample.isFinite(), sample.toString());

	setRgbPixel(xPx, yPx, spectrum_sample_to_rgb(sample));
}

std::unique_ptr<TSamplingFilm<math::Spectrum>> HdrRgbFilm::makeCopy(const bool shouldCopySamples) const
{
	auto copiedFilm = std::make_unique<HdrRgbFilm>(
		getActualResPx().x(),
		getActualResPx().y(),
		getEffectiveWindowPx(),
		getFilter());

	copiedFilm->setSoftEdge(isSoftEdged());
	
	if(shouldCopySamples)
	{
		copiedFilm->mergeWith(*this);
	}

	return copiedFilm;
}

void HdrRgbFilm::addRgbSample(
	const float64         xPx, 
	const float64         yPx, 
	const math::Vector3D& rgb)
{
	const math::TVector2<float64> samplePosPx(xPx, yPx);

	// Compute filter bounds
	math::TVector2<float64> filterMin(samplePosPx.sub(getFilter().getHalfSizePx()));
	math::TVector2<float64> filterMax(samplePosPx.add(getFilter().getHalfSizePx()));

	// Reduce to effective bounds
	filterMin = filterMin.max(math::TVector2<float64>(getEffectiveWindowPx().getMinVertex()));
	filterMax = filterMax.min(math::TVector2<float64>(getEffectiveWindowPx().getMaxVertex()));

	// Compute pixel index bounds (exclusive on x1y1)
	math::TVector2<int64> x0y0(filterMin.sub(0.5).ceil());
	math::TVector2<int64> x1y1(filterMax.sub(0.5).floor());
	x1y1.x() += 1;
	x1y1.y() += 1;

	const float64 rgbR = rgb.r();
	const float64 rgbG = rgb.g();
	const float64 rgbB = rgb.b();
	for(int64 y = x0y0.y(); y < x1y1.y(); ++y)
	{
		for(int64 x = x0y0.x(); x < x1y1.x(); ++x)
		{
			// TODO: factor out the -0.5 part
			const float64 filterX      = x - (xPx - 0.5);
			const float64 filterY      = y - (yPx - 0.5);
			const auto    filterWeight = getFilter().evaluate(filterX, filterY);

			const auto sensorX      = x - getEffectiveWindowPx().getMinVertex().x();
			const auto sensorY      = y - getEffectiveWindowPx().getMinVertex().y();
			const auto sensorIndex  = sensorY * getEffectiveResPx().x() + sensorX;

			m_pixelRadianceSensors[sensorIndex].accuR      += rgbR * filterWeight;
			m_pixelRadianceSensors[sensorIndex].accuG      += rgbG * filterWeight;
			m_pixelRadianceSensors[sensorIndex].accuB      += rgbB * filterWeight;
			m_pixelRadianceSensors[sensorIndex].accuWeight += filterWeight;
		}
	}
}

void HdrRgbFilm::setRgbPixel(
	const float64         xPx, 
	const float64         yPx, 
	const math::Vector3D& rgb,
	const float64         weight)
{
	const auto filmX = std::min(static_cast<int64>(xPx), static_cast<int64>(getActualResPx().x()) - 1);
	const auto filmY = std::min(static_cast<int64>(yPx), static_cast<int64>(getActualResPx().y()) - 1);

	const auto ix    = filmX - getEffectiveWindowPx().getMinVertex().x();
	const auto iy    = filmY - getEffectiveWindowPx().getMinVertex().y();
	const auto index = iy * static_cast<std::size_t>(getEffectiveResPx().x()) + ix;

	m_pixelRadianceSensors[index].accuR      = rgb.r() * weight;
	m_pixelRadianceSensors[index].accuG      = rgb.g() * weight;
	m_pixelRadianceSensors[index].accuB      = rgb.b() * weight;
	m_pixelRadianceSensors[index].accuWeight = weight;
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
		PH_DEFAULT_LOG(Warning,
			"Input frame dimension mismatch when developing HdrRgbFilm (film size: {}, frame size: {})",
			getActualResPx(), out_frame.getSizePx());
		return;
	}

	// We only need to develop the pixels within this region
	math::TAABB2D<int64> frameWindow(getEffectiveWindowPx());
	frameWindow.intersectWith(regionPx);

	for(int64 y = frameWindow.getMinVertex().y(); y < frameWindow.getMaxVertex().y(); ++y)
	{
		for(int64 x = frameWindow.getMinVertex().x(); x < frameWindow.getMaxVertex().x(); ++x)
		{
			const auto sensorX      = x - getEffectiveWindowPx().getMinVertex().x();
			const auto sensorY      = y - getEffectiveWindowPx().getMinVertex().y();
			const auto sensorIndex  = sensorY * getEffectiveResPx().x() + sensorX;
			const auto sensorWeight = m_pixelRadianceSensors[sensorIndex].accuWeight;

			// Prevent division by zero
			const auto rcpWeight = sensorWeight == 0.0 ? 0.0 : 1.0 / sensorWeight;

			const auto sensorR = m_pixelRadianceSensors[sensorIndex].accuR * rcpWeight;
			const auto sensorG = m_pixelRadianceSensors[sensorIndex].accuG * rcpWeight;
			const auto sensorB = m_pixelRadianceSensors[sensorIndex].accuB * rcpWeight;

			// TODO: prevent negative pixel
			out_frame.setPixel(
				static_cast<uint32>(x), static_cast<uint32>(y),
				HdrRgbFrame::PixelType({
					static_cast<HdrComponent>(sensorR), 
					static_cast<HdrComponent>(sensorG), 
					static_cast<HdrComponent>(sensorB)}));
		}
	}
}

void HdrRgbFilm::clear()
{
	clearRadianceSensors();
}

void HdrRgbFilm::mergeWith(const TSamplingFilm<math::Spectrum>& other)
{
	auto const otherPtr = dynamic_cast<const HdrRgbFilm*>(&other);
	PH_ASSERT(otherPtr != this);
	if(!otherPtr)
	{
		TSamplingFilm<math::Spectrum>::mergeWith(other);
		return;
	}

	math::TAABB2D<int64> validRegion(this->getEffectiveWindowPx());
	validRegion.intersectWith(otherPtr->getEffectiveWindowPx());

	for(int64 y = validRegion.getMinVertex().y(); y < validRegion.getMaxVertex().y(); ++y)
	{
		const std::size_t thisY = y - this->getEffectiveWindowPx().getMinVertex().y();
		const std::size_t otherY = y - otherPtr->getEffectiveWindowPx().getMinVertex().y();
		const std::size_t thisBaseIndex = thisY * static_cast<std::size_t>(this->getEffectiveResPx().x());
		const std::size_t otherBaseIndex = otherY * static_cast<std::size_t>(otherPtr->getEffectiveResPx().x());

		for(int64 x = validRegion.getMinVertex().x(); x < validRegion.getMaxVertex().x(); ++x)
		{
			const std::size_t thisX = x - this->getEffectiveWindowPx().getMinVertex().x();
			const std::size_t otherX = x - otherPtr->getEffectiveWindowPx().getMinVertex().x();
			const std::size_t thisI = thisBaseIndex + thisX;
			const std::size_t otherI = otherBaseIndex + otherX;

			m_pixelRadianceSensors[thisI].accuR      += otherPtr->m_pixelRadianceSensors[otherI].accuR;
			m_pixelRadianceSensors[thisI].accuG      += otherPtr->m_pixelRadianceSensors[otherI].accuG;
			m_pixelRadianceSensors[thisI].accuB      += otherPtr->m_pixelRadianceSensors[otherI].accuB;
			m_pixelRadianceSensors[thisI].accuWeight += otherPtr->m_pixelRadianceSensors[otherI].accuWeight;
		}
	}
}

void HdrRgbFilm::setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow)
{
	TSamplingFilm<math::Spectrum>::setEffectiveWindowPx(effectiveWindow);

	resizeRadianceSensorBuffer();
}

}// end namespace
