#pragma once

#include "Core/Filmic/Film.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"

#include <vector>
#include <functional>
#include <memory>

namespace ph
{

class InputPacket;
class SampleFilter;

// TODO: not registered (how should we register class template)
template<typename Sample>
class TSamplingFilm : public Film, public TCommandInterface<TSamplingFilm<Sample>>
{
public:
	TSamplingFilm(
		int64 actualWidthPx, int64 actualHeightPx,
		const SampleFilter& filter);

	TSamplingFilm(
		int64 actualWidthPx, int64 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter& filter);

	~TSamplingFilm() override;

	virtual void addSample(float64 xPx, float64 yPx, const Sample& sample) = 0;

	// Generates a child film with the same actual dimensions and filter as parent's, 
	// but potentially has a different effective window.
	// A child film can be merged into its parent by calling mergeToParent().
	//
	virtual std::unique_ptr<TSamplingFilm> genChild(const TAABB2D<int64>& effectiveWindowPx) = 0;

	virtual void clear() override = 0;

	inline void mergeToParent() const
	{
		m_merger();
	}

	inline const TVector2<float64>& getSampleResPx() const
	{
		return m_sampleResPx;
	}

	inline const TAABB2D<float64>& getSampleWindowPx() const
	{
		return m_sampleWindowPx;
	}

protected:
	TVector2<float64> m_sampleResPx;
	TAABB2D<float64>  m_sampleWindowPx;

	SampleFilter          m_filter;
	std::function<void()> m_merger;

private:
	void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const override = 0;

	void calcSampleDimensions();

	static std::function<void()> makeDefaultMerger();

// command interface
public:
	explicit TSamplingFilm(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

#include "Core/Filmic/TSamplingFilm.ipp"