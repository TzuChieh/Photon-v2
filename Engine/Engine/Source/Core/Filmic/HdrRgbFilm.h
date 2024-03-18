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

	void addSample(float64 xPx, float64 yPx, const math::Spectrum& sample) override;
	void setPixel(float64 xPx, float64 yPx, const math::Spectrum& sample) override;
	void clear() override;
	void setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow) override;

	/*! @brief Add a pixel sample.
	@param xPx The pixel's x coordinate.
	@param yPx The pixel's y coordinate.
	@param rgb The RGB values of the pixel.
	*/
	///@{
	void addRgbSample(float64 xPx, float64 yPx, const math::Vector3R& rgb);
	void addRgbSample(float64 xPx, float64 yPx, const math::Vector3D& rgb);
	///@}

	/*! @brief Set a pixel directly.
	Filter is not applied when setting a pixel directly (effectively applying a box filter).
	The pixel coordinate is not integral as the implementation may choose to store the values in
	higher resolution (e.g., super sampling).
	@param xPx The pixel's x coordinate.
	@param yPx The pixel's y coordinate.
	@param rgb The RGB values of the pixel.
	@param weight The weight of the pixel.
	*/
	///@{
	void setRgbPixel(float64 xPx, float64 yPx, const math::Vector3R& rgb, float64 weight = 1);
	void setRgbPixel(float64 xPx, float64 yPx, const math::Vector3D& rgb, float64 weight = 1);
	///@}

	void mergeWith(const HdrRgbFilm& other);

private:
	void developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const override;

	void clearRadianceSensors();
	void resizeRadianceSensorBuffer();

	std::vector<RadianceSensor> m_pixelRadianceSensors;
};

inline void HdrRgbFilm::addRgbSample(
	const float64         xPx, 
	const float64         yPx, 
	const math::Vector3R& rgb)
{
	addRgbSample(xPx, yPx, math::Vector3D(rgb));
}

inline void HdrRgbFilm::setRgbPixel(
	const float64         xPx,
	const float64         yPx,
	const math::Vector3R& rgb,
	const float64         weight)
{
	setRgbPixel(xPx, yPx, math::Vector3D(rgb), weight);
}

inline void HdrRgbFilm::clearRadianceSensors()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), RadianceSensor());
}

inline void HdrRgbFilm::resizeRadianceSensorBuffer()
{
	m_pixelRadianceSensors.resize(getEffectiveWindowPx().getArea());
}

}// end namespace ph
