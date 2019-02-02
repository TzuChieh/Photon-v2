#pragma once

#include "Core/Filmic/SamplingFilmBase.h"
#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"

#include <vector>
#include <functional>
#include <memory>

namespace ph
{

// TODO: not registered (how should we register class template)
template<typename Sample>
class TSamplingFilm : public SamplingFilmBase
{
public:
	using SamplingFilmBase::SamplingFilmBase;

	virtual void addSample(float64 xPx, float64 yPx, const Sample& sample) = 0;
	virtual std::unique_ptr<TSamplingFilm> genSamplingChild(const TAABB2D<int64>& effectiveWindowPx) = 0;

	void clear() override = 0;

	std::unique_ptr<SamplingFilmBase> genChild(const TAABB2D<int64>& effectiveWindowPx) override;

private:
	void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const override = 0;
};

}// end namespace ph

#include "Core/Filmic/TSamplingFilm.ipp"