#pragma once

#include "Core/Filmic/Film.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Math/Geometry/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Filmic/SamplingFilmDimensions.h"

#include <vector>
#include <functional>
#include <memory>

namespace ph
{

/*! @brief A camera film that receives any quantity, for sampling-based rendering techniques.
*/
template<typename Sample>
class TSamplingFilm : public Film
{
public:
	TSamplingFilm() = default;

	TSamplingFilm(
		int64                       actualWidthPx, 
		int64                       actualHeightPx,
		const SampleFilter&         filter);

	TSamplingFilm(
		int64                       actualWidthPx, 
		int64                       actualHeightPx,
		const math::TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter&         filter);

	/*! @brief Applies filter to sample value and add to the film.
	Generally, this method can produce higher quality result by reconstruct the image function with
	a filter.
	@param xPx The x coordinate in raster space.
	@param yPx The y coordinate in raster space.
	@param sample The value to add as a sample.
	*/
	virtual void addSample(float64 xPx, float64 yPx, const Sample& sample) = 0;

	/*! @brief Let the pixel developes to the specified sample value.
	An interface for using the film as an ordinary image. This method may disturb sample weights.
	Using this method with `addSample()` may result in image artifacts unless the implementation
	says otherwise.
	@param xPx The x coordinate in raster space.
	@param yPx The y coordinate in raster space.
	@param sample The value the pixel will develop to.
	*/
	virtual void setPixel(float64 xPx, float64 yPx, const Sample& sample) = 0;

	void clear() override = 0;

	void setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow) override;

	/*! @brief Whether to increase the sampling window converage for prettier boundaries in developed frame.
	In renderings where the image is divided into tiles and sampled independently, the resulting image
	may have visible artifacts along tile boundaries. This can be alleviated by increasing the size of
	each tile, so edge pixels can receive more information, reducing their variance. This costs slightly
	more memory, and tiles will have overlapping sampling regions (may need more care for concurrency).
	@param useSoftEdge Whether prettier boundaries are needed or not. By default soft edge is enabled
	with maximum softness (1). If `false`, the value of `softness` will be ignored and set to 0.
	@param softness A value in `[0, 1]` to indicate the amount of increased size relative to the filter
	used. For a value of `1`, the amount of expansion will be the radius of the filter.
	@note Using soft edge or not will not affect effective window in any way.
	*/
	void setSoftEdge(bool useSoftEdge, float32 softness = 1.0f);

	math::TVector2<float64> getSampleResPx() const;
	const math::TAABB2D<float64>& getSampleWindowPx() const;
	const SampleFilter& getFilter() const;
	SamplingFilmDimensions getDimensions() const;
	bool isSoftEdged() const;

private:
	void developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const override = 0;

	void updateSampleDimensions();

	SampleFilter           m_filter;
	math::TAABB2D<float64> m_sampleWindowPx;
	float32                m_softness;
};

}// end namespace ph

#include "Core/Filmic/TSamplingFilm.ipp"
