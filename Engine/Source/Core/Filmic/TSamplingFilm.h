#pragma once

#include "Core/Filmic/Film.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Filmic/SamplingFilmDimensions.h"

#include <vector>
#include <functional>
#include <memory>

namespace ph
{

template<typename Sample>
class TSamplingFilm : public Film
{
public:
	TSamplingFilm() = default;

	TSamplingFilm(
		int64               actualWidthPx, 
		int64               actualHeightPx,
		const SampleFilter& filter);

	TSamplingFilm(
		int64                 actualWidthPx, 
		int64                 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter&   filter);

	TSamplingFilm(TSamplingFilm&& other);

	virtual void addSample(float64 xPx, float64 yPx, const Sample& sample) = 0;

	void clear() override = 0;

	void setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow) override;

	TVector2<float64>       getSampleResPx() const;
	const TAABB2D<float64>& getSampleWindowPx() const;
	const SampleFilter&     getFilter() const;
	SamplingFilmDimensions  getDimensions() const;

	TSamplingFilm& operator = (TSamplingFilm&& other);

private:
	void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const override = 0;

	SampleFilter     m_filter;
	TAABB2D<float64> m_sampleWindowPx;

	void updateSampleDimensions();
};

// In-header Implementations:

template<typename Sample>
inline const SampleFilter& TSamplingFilm<Sample>::getFilter() const
{
	return m_filter;
}

template<typename Sample>
inline TVector2<float64> TSamplingFilm<Sample>::getSampleResPx() const
{
	return {m_sampleWindowPx.getWidth(), m_sampleWindowPx.getHeight()};
}

template<typename Sample>
inline const TAABB2D<float64>& TSamplingFilm<Sample>::getSampleWindowPx() const
{
	return m_sampleWindowPx;
}

template<typename Sample>
inline SamplingFilmDimensions TSamplingFilm<Sample>::getDimensions() const
{
	return {getActualResPx(), getEffectiveWindowPx(), getSampleWindowPx()};
}

}// end namespace ph

#include "Core/Filmic/TSamplingFilm.ipp"