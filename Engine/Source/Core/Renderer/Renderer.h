#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Renderer/ERegionStatus.h"
#include "Core/Renderer/Statistics.h"
#include "Frame/frame_fwd.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <deque>

namespace ph
{

class Description;

class Renderer
{
public:
	typedef TAABB2D<int64> Region;

public:
	Renderer();
	virtual ~Renderer() = 0;

	virtual void init(const Description& description) = 0;
	virtual bool getNewWork(uint32 workerId, RenderWork* out_work) = 0;
	virtual void submitWork(uint32 workerId, const RenderWork& work, bool isUpdating) = 0;
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) = 0;
	virtual void asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region) = 0;

	void render(const Description& description);
	void setNumRenderThreads(const uint32 numThreads);
	void asyncQueryStatistics(float32* out_percentageProgress, 
	                          float32* out_samplesPerSecond) const;

protected:
	uint32 m_numThreads;

private:
	std::vector<RenderWorker> m_workers;
};

}// end namespace ph