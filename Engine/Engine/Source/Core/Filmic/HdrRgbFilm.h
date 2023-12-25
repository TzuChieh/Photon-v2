#pragma once

#include "Math/math_fwd.h"
#include "Core/Receiver/RadianceSensor.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

#include <vector>
#include <algorithm>

namespace ph
{

/*! @brief A film that records RGB values internally.
Although film samples are added as spectral values, they are automatically converted to linear sRGB
values for storage. 
*/
class HdrRgbFilm : public TSamplingFilm<math::Spectrum>
{
public:
	HdrRgbFilm() = default;

	HdrRgbFilm(
		int64                       actualWidthPx, 
		int64                       actualHeightPx,
		const SampleFilter&         filter);

	HdrRgbFilm(
		int64                       actualWidthPx, 
		int64                       actualHeightPx,
		const math::TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter&         filter);

	void addSample(float64 xPx, float64 yPx, const math::Spectrum& spectrum) override;
	void clear() override;
	void setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow) override;

	void addRgbSample(float64 xPx, float64 yPx, const math::Vector3R& rgb);
	void mergeWith(const HdrRgbFilm& other);

	// HACK
	void setPixel(float64 xPx, float64 yPx, const math::Spectrum& spectrum);

private:
	void developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const override;

	void clearRadianceSensors();
	void resizeRadianceSensorBuffer();

	std::vector<RadianceSensor> m_pixelRadianceSensors;
};

inline void HdrRgbFilm::clearRadianceSensors()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), RadianceSensor());
}

inline void HdrRgbFilm::resizeRadianceSensorBuffer()
{
	m_pixelRadianceSensors.resize(getEffectiveWindowPx().getArea());
}

}// end namespace ph
