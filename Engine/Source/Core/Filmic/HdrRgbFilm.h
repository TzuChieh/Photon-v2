#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Camera/RadianceSensor.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Quantity/SpectralStrength.h"

#include <vector>
#include <memory>

namespace ph
{

/*
	A film that records RGB values internally. Although film samples are
	added as spectral values, they are automatically converted to linear sRGB
	values for storage. 
*/

// TODO: move and copy
class HdrRgbFilm : public TSamplingFilm<SpectralStrength>
{
public:
	HdrRgbFilm() = default;

	HdrRgbFilm(
		int64               actualWidthPx, 
		int64               actualHeightPx,
		const SampleFilter& filter);

	HdrRgbFilm(
		int64                       actualWidthPx, 
		int64                       actualHeightPx,
		const math::TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter&         filter);

	HdrRgbFilm(HdrRgbFilm&& other);

	void addSample(float64 xPx, float64 yPx, const SpectralStrength& spectrum) override;
	void clear() override;
	void setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow) override;

	void addSample(float64 xPx, float64 yPx, const math::Vector3R& rgb);
	void mergeWith(const HdrRgbFilm& other);

	HdrRgbFilm& operator = (HdrRgbFilm&& other);

	// HACK
	void setPixel(float64 xPx, float64 yPx, const SpectralStrength& spectrum);

private:
	void developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const override;

	std::vector<RadianceSensor> m_pixelRadianceSensors;

	void resizeRadianceSensorBuffer();
};

}// end namespace ph
