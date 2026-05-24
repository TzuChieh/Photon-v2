#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Core/Filmic/TSamplingFilm.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/statistics.h"

#include <Common/primitive_type.h>

#include <vector>
#include <algorithm>

namespace ph
{

/*! @brief A film that records per-channel sample variance internally.
Although film samples are added as spectral values, they are automatically converted to linear sRGB
values for storage.
*/
class HdrRgbVarianceFilm : public TSamplingFilm<math::Spectrum>
{
public:
	HdrRgbVarianceFilm() = default;

	HdrRgbVarianceFilm(
		int64                       actualWidthPx,
		int64                       actualHeightPx,
		const SampleFilter&         filter);

	HdrRgbVarianceFilm(
		int64                       actualWidthPx,
		int64                       actualHeightPx,
		const math::TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter&         filter);

	void addSample(float64 xPx, float64 yPx, const math::Spectrum& sample) override;
	void setPixel(float64 xPx, float64 yPx, const math::Spectrum& sample) override;
	std::unique_ptr<TSamplingFilm<math::Spectrum>> makeCopy(bool shouldCopySamples) const override;
	void mergeWith(const TSamplingFilm<math::Spectrum>& other) override;
	void clear() override;
	void setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow) override;

	///@{
	void addRgbSample(float64 xPx, float64 yPx, const math::Vector3R& rgb);
	void addRgbSample(float64 xPx, float64 yPx, const math::Vector3D& rgb);
	///@}

	///@{
	void setRgbPixel(float64 xPx, float64 yPx, const math::Vector3R& rgb, float64 weight = 1);
	void setRgbPixel(float64 xPx, float64 yPx, const math::Vector3D& rgb, float64 weight = 1);
	///@}

private:
	struct VarianceSensor
	{
		struct WelfordAccumulator
		{
			inline void addSample(const float64 sample, const float64 weight)
			{
				math::weighted_welford_add(sample, weight, weightSum, squaredWeightSum, mean, squaredDiffSum);
			}

			inline void merge(const WelfordAccumulator& other)
			{
				math::weighted_welford_merge(
					other.weightSum, other.squaredWeightSum, other.mean, other.squaredDiffSum,
					weightSum, squaredWeightSum, mean, squaredDiffSum);
			}

			inline float64 getSampleVariance() const
			{
				return math::weighted_welford_sample_variance(
					weightSum, squaredWeightSum, squaredDiffSum);
			}

			float64 weightSum = 0;
			float64 squaredWeightSum = 0;
			float64 mean = 0;
			float64 squaredDiffSum = 0;
		};

		WelfordAccumulator r;
		WelfordAccumulator g;
		WelfordAccumulator b;
	};

	void developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const override;

	void clearVarianceSensors();
	void resizeVarianceSensorBuffer();

	std::vector<VarianceSensor> m_pixelVarianceSensors;
};

inline void HdrRgbVarianceFilm::addRgbSample(
	const float64         xPx,
	const float64         yPx,
	const math::Vector3R& rgb)
{
	addRgbSample(xPx, yPx, math::Vector3D(rgb));
}

inline void HdrRgbVarianceFilm::setRgbPixel(
	const float64         xPx,
	const float64         yPx,
	const math::Vector3R& rgb,
	const float64         weight)
{
	setRgbPixel(xPx, yPx, math::Vector3D(rgb), weight);
}

inline void HdrRgbVarianceFilm::clearVarianceSensors()
{
	std::fill(m_pixelVarianceSensors.begin(), m_pixelVarianceSensors.end(), VarianceSensor());
}

inline void HdrRgbVarianceFilm::resizeVarianceSensorBuffer()
{
	m_pixelVarianceSensors.resize(getEffectiveWindowPx().getArea());
}

}// end namespace ph
