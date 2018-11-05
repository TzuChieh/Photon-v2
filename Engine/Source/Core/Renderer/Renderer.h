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
#include "Core/Renderer/AttributeTags.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Renderer/RenderStates.h"
#include "Core/Renderer/Region/Region.h"
#include "Core/Renderer/Region/RegionScheduler.h"
#include "Common/assertion.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <deque>

namespace ph
{

class SdlResourcePack;
class InputPacket;
class RenderWorker;

class Renderer: public TCommandInterface<Renderer>
{
public:
	virtual ~Renderer();

	virtual AttributeTags supportedAttributes() const = 0;
	virtual void init(const SdlResourcePack& data) = 0;
	virtual void render() = 0;
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) = 0;
	virtual RenderStates asyncQueryRenderStates() = 0;

	// TODO: this can somehow combine with its async version
	virtual void develop(HdrRgbFrame& out_frame, EAttribute attribute) = 0;

	// TODO: remove the word "Film"
	virtual void asyncDevelopFilmRegion(
		HdrRgbFrame&  out_frame, 
		const Region& region, 
		EAttribute    attribute) = 0;

	void start(const SdlResourcePack& description);
	void setNumWorkers(uint32 numWorkers);
	void asyncQueryStatistics(float32* out_percentageProgress, 
	                          float32* out_samplesPerSecond);
	RenderProgress asyncQueryWorkerProgress(uint32 workerId);

	uint32           getNumWorkers()       const;
	uint32           getRenderWidthPx()    const;
	uint32           getRenderHeightPx()   const;
	TAABB2D<int64>   getRenderWindowPx()   const;
	RegionScheduler* getRegionScheduler()  const;

private:
	uint32         m_numWorkers;
	uint32         m_widthPx;
	uint32         m_heightPx;
	TAABB2D<int64> m_windowPx;

	std::vector<RenderWorker> m_workers;
	std::unique_ptr<RegionScheduler> m_regionScheduler;

// command interface
public:
	explicit Renderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline uint32 Renderer::getNumWorkers() const
{
	return m_numWorkers;
}

inline uint32 Renderer::getRenderWidthPx() const
{
	return m_widthPx;
}

inline uint32 Renderer::getRenderHeightPx() const
{
	return m_heightPx;
}

inline TAABB2D<int64> Renderer::getRenderWindowPx() const
{
	return m_windowPx;
}

inline RegionScheduler* Renderer::getRegionScheduler() const
{
	PH_ASSERT(m_regionScheduler);

	return m_regionScheduler.get();
}

}// end namespace ph