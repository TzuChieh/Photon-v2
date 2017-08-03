#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Camera/Camera.h"
#include "Core/Bound/TAABB2D.h"
#include "Core/Renderer/ERegionStatus.h"

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
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) = 0;
	virtual bool getNewWork(RenderWork* out_work) = 0;
	virtual void submitWork(const RenderWork& work, bool isUpdating) = 0;

	void render(const Description& description);
	void setNumRenderThreads(const uint32 numThreads);
	float32 asyncQueryPercentageProgress() const;
	float32 asyncQuerySampleFrequency() const;

protected:
	uint32 m_numThreads;

private:
	std::vector<RenderWorker> m_workers;
};

}// end namespace ph