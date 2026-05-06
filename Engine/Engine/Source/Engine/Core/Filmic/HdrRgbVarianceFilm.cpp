#include "Engine/Core/Filmic/HdrRgbVarianceFilm.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Frame/TFrame.h"
#include "Engine/Core/Filmic/SampleFilter.h"
#include "Engine/Math/statistics.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cstddef>
#include <algorithm>

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

HdrRgbVarianceFilm::HdrRgbVarianceFilm(
	const int64                 actualWidthPx,
	const int64                 actualHeightPx,
	const SampleFilter&         filter)

	: HdrRgbVarianceFilm(
		actualWidthPx,
		actualHeightPx,
		math::TAABB2D<int64>(
			math::TVector2<int64>(0, 0),
			math::TVector2<int64>(actualWidthPx, actualHeightPx)),
		filter)
{}

HdrRgbVarianceFilm::HdrRgbVarianceFilm(
	const int64                 actualWidthPx,
	const int64                 actualHeightPx,
	const math::TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter&         filter)

	: TSamplingFilm<math::Spectrum>(
		actualWidthPx,
		actualHeightPx,
		effectiveWindowPx,
		filter)

	, m_pixelVarianceSensors()
{
	resizeVarianceSensorBuffer();
	clearVarianceSensors();
}

void HdrRgbVarianceFilm::addSample(
	const float64         xPx,
	const float64         yPx,
	const math::Spectrum& sample)
{
	PH_ASSERT_MSG(sample.isFinite(), sample.toString());

	addRgbSample(xPx, yPx, spectrum_sample_to_rgb(sample));
}

void HdrRgbVarianceFilm::setPixel(
	const float64         xPx,
	const float64         yPx,
	const math::Spectrum& sample)
{
	PH_ASSERT_MSG(sample.isFinite(), sample.toString());

	setRgbPixel(xPx, yPx, spectrum_sample_to_rgb(sample));
}

void HdrRgbVarianceFilm::addRgbSample(
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

	// Keep this weighting close to `HdrRgbFilm` for comparable sample statistics.
	// Non-positive weight (which should be rare) is clamped to 0 for variance stability.
	for(int64 y = x0y0.y(); y < x1y1.y(); ++y)
	{
		for(int64 x = x0y0.x(); x < x1y1.x(); ++x)
		{
			// TODO: factor out the -0.5 part
			const float64 filterX = x - (xPx - 0.5);
			const float64 filterY = y - (yPx - 0.5);

			const auto filterWeight = static_cast<float64>(getFilter().evaluate(filterX, filterY));
			if(filterWeight <= 0)
			{
				continue;
			}

			const auto sensorX      = x - getEffectiveWindowPx().getMinVertex().x();
			const auto sensorY      = y - getEffectiveWindowPx().getMinVertex().y();
			const auto sensorIndex  = sensorY * getEffectiveResPx().x() + sensorX;
			auto&      sensor       = m_pixelVarianceSensors[sensorIndex];

			sensor.r.addSample(static_cast<float64>(rgb.r()), filterWeight);
			sensor.g.addSample(static_cast<float64>(rgb.g()), filterWeight);
			sensor.b.addSample(static_cast<float64>(rgb.b()), filterWeight);
		}
	}
}

void HdrRgbVarianceFilm::setRgbPixel(
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
	auto&      sensor = m_pixelVarianceSensors[index];

	sensor = VarianceSensor();

	// For stability, see `addRgbSample()` for details
	if(weight <= 0)
	{
		return;
	}

	sensor.r.addSample(static_cast<float64>(rgb.r()), static_cast<float64>(weight));
	sensor.g.addSample(static_cast<float64>(rgb.g()), static_cast<float64>(weight));
	sensor.b.addSample(static_cast<float64>(rgb.b()), static_cast<float64>(weight));
}

void HdrRgbVarianceFilm::developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const
{
	if(out_frame.widthPx()  != getActualResPx().x() ||
	   out_frame.heightPx() != getActualResPx().y())
	{
		PH_DEFAULT_LOG(Warning,
			"Input frame dimension mismatch when developing HdrRgbVarianceFilm (film size: {}, frame size: {})",
			getActualResPx(), out_frame.getSizePx());
		return;
	}

	math::TAABB2D<int64> frameWindow(getEffectiveWindowPx());
	frameWindow.intersectWith(regionPx);

	for(int64 y = frameWindow.getMinVertex().y(); y < frameWindow.getMaxVertex().y(); ++y)
	{
		for(int64 x = frameWindow.getMinVertex().x(); x < frameWindow.getMaxVertex().x(); ++x)
		{
			const auto  sensorX     = x - getEffectiveWindowPx().getMinVertex().x();
			const auto  sensorY     = y - getEffectiveWindowPx().getMinVertex().y();
			const auto  sensorIndex = sensorY * getEffectiveResPx().x() + sensorX;
			const auto& sensor      = m_pixelVarianceSensors[sensorIndex];

			out_frame.setPixel(
				static_cast<uint32>(x), static_cast<uint32>(y),
				HdrRgbFrame::PixelType({
					static_cast<HdrComponent>(sensor.r.getUnbiasedVariance()),
					static_cast<HdrComponent>(sensor.g.getUnbiasedVariance()),
					static_cast<HdrComponent>(sensor.b.getUnbiasedVariance())}));
		}
	}
}

void HdrRgbVarianceFilm::clear()
{
	clearVarianceSensors();
}

void HdrRgbVarianceFilm::mergeWith(const TSamplingFilm<math::Spectrum>& other)
{
	auto const otherPtr = dynamic_cast<const HdrRgbVarianceFilm*>(&other);
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
			auto&             thisSensor = m_pixelVarianceSensors[thisI];
			const auto&       otherSensor = otherPtr->m_pixelVarianceSensors[otherI];

			thisSensor.r.merge(otherSensor.r);
			thisSensor.g.merge(otherSensor.g);
			thisSensor.b.merge(otherSensor.b);
		}
	}
}

void HdrRgbVarianceFilm::setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow)
{
	TSamplingFilm<math::Spectrum>::setEffectiveWindowPx(effectiveWindow);

	resizeVarianceSensorBuffer();
}

}// end namespace ph
