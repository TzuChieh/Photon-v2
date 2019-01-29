#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/Region/Region.h"
#include "Common/primitive_type.h"
#include "Core/Renderer/Region/GridScheduler.h"

#include <memory>
#include <queue>
#include <cstddef>
#include <vector>

namespace ph
{

class AdaptiveSamplingRenderer : public SamplingRenderer, public TCommandInterface<AdaptiveSamplingRenderer>
{
public:
	void doUpdate(const SdlResourcePack& data) override;

	bool supplyWork(
		uint32 workerId,
		SamplingRenderWork& work,
		float* out_suppliedFraction) override;

	void submitWork(
		uint32 workerId,
		SamplingRenderWork& work,
		float* out_submittedFraction) override;

private:
	struct WorkingRegion
	{
		Region region;
		uint32 numWorkers;
	};

	real m_splitThreshold;
	real m_terminateThreshold;
	std::size_t m_numPathsPerRegion;

	std::vector<WorkingRegion> m_workingRegions;
	std::vector<uint32> m_workerIdToWorkingRegion;
	std::queue<Region> m_pendingRegions;
	GridScheduler m_currentGrid;

// command interface
public:
	explicit AdaptiveSamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
