#pragma once

#include "Core/Filmic/Film.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"
#include "Core/Filmic/SampleFilter.h"

#include <vector>
#include <functional>
#include <memory>

namespace ph
{

class InputPacket;
class SampleFilter;

class SamplingFilmBase : public Film, public TCommandInterface<SamplingFilmBase>
{
public:
	using Merger = std::function<void()>;

	SamplingFilmBase(
		int64 actualWidthPx, int64 actualHeightPx,
		const SampleFilter& filter);

	SamplingFilmBase(
		int64 actualWidthPx, int64 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter& filter);

	~SamplingFilmBase() override;

	// Generates a child film with the same actual dimensions and filter as parent's, 
	// but potentially has a different effective window.
	// A child film can be merged into its parent by calling mergeToParent().
	//
	virtual std::unique_ptr<SamplingFilmBase> genChild(const TAABB2D<int64>& effectiveWindowPx) = 0;

	void clear() override = 0;

	void setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow) override;

	void setMerger(const Merger& merger);
	void mergeToParent() const;
	TVector2<float64> getSampleResPx() const;
	const TAABB2D<float64>& getSampleWindowPx() const;

protected:
	// TODO: move to private
	SampleFilter          m_filter;

private:
	Merger m_merger;

	void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const override = 0;

	TAABB2D<float64> m_sampleWindowPx;

	void calcSampleDimensions();
	static Merger makeNoOpMerger();

// command interface
public:
	explicit SamplingFilmBase(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph