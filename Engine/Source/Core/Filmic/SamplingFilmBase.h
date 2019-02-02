#pragma once

#include "Core/Filmic/Film.h"
#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"
#include "Core/Filmic/SampleFilter.h"

#include <vector>
#include <functional>
#include <memory>

namespace ph
{

class SamplingFilmBase : public Film
{
public:
	SamplingFilmBase(
		int64 actualWidthPx, int64 actualHeightPx,
		const SampleFilter& filter);

	SamplingFilmBase(
		int64 actualWidthPx, int64 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter& filter);

	// Generates a child film with the same actual dimensions and filter as parent's, 
	// but potentially with a different effective window.
	// A child film can be merged into its parent by calling mergeToParent().
	//
	virtual std::unique_ptr<SamplingFilmBase> genChild(const TAABB2D<int64>& effectiveWindowPx) = 0;

	void clear() override = 0;

	void setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow) override;

	void mergeToParent() const;
	TVector2<float64> getSampleResPx() const;
	const TAABB2D<float64>& getSampleWindowPx() const;
	const SampleFilter& getFilter() const;

protected:
	using Merger = std::function<void()>;

	void setMerger(const Merger& merger);

private:
	void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const override = 0;

	SampleFilter     m_filter;
	Merger           m_merger;
	TAABB2D<float64> m_sampleWindowPx;

	void calcSampleDimensions();
	static Merger makeNoOpMerger();
};

// In-header Implementations:

inline const SampleFilter& SamplingFilmBase::getFilter() const
{
	return m_filter;
}

}// end namespace ph