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
#include "Core/Estimator/Attribute/AttributeTags.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Estimator/Attribute/EAttribute.h"
#include "Core/Renderer/RenderStates.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <deque>

namespace ph
{

class Description;
class InputPacket;
class RenderWorker;

class Renderer: public TCommandInterface<Renderer>
{
public:
	typedef TAABB2D<int64> Region;

public:
	virtual ~Renderer() = 0;

	virtual AttributeTags supportedAttributes() const = 0;
	virtual void init(const Description& description) = 0;
	virtual bool asyncSupplyWork(RenderWorker& worker) = 0;
	virtual void asyncSubmitWork(RenderWorker& worker) = 0;
	virtual ERegionStatus asyncPollUpdatedRegion(Region* out_region) = 0;
	virtual RenderStates asyncQueryRenderStates() = 0;

	virtual void develop(HdrRgbFrame& out_frame, EAttribute attribute) = 0;

	virtual void asyncDevelopFilmRegion(
		HdrRgbFrame&  out_frame, 
		const Region& region, 
		EAttribute    attribute) = 0;


	void render(const Description& description);
	void setNumRenderThreads(const uint32 numThreads);
	void asyncQueryStatistics(float32* out_percentageProgress, 
	                          float32* out_samplesPerSecond);

	uint32         getNumRenderThreads() const;
	uint32         getRenderWidthPx()    const;
	uint32         getRenderHeightPx()   const;
	TAABB2D<int64> getRenderWindowPx()   const;

private:
	uint32         m_numThreads;
	uint32         m_widthPx;
	uint32         m_heightPx;
	TAABB2D<int64> m_windowPx;

	std::vector<RenderWorker> m_workers;

// command interface
public:
	explicit Renderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline uint32 Renderer::getNumRenderThreads() const
{
	return m_numThreads;
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

}// end namespace ph