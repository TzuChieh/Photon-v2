#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/Region/WorkScheduler.h"
#include "Core/Renderer/Region/WorkUnit.h"

#include <memory>
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
	std::unique_ptr<WorkScheduler> m_workScheduler;
	std::vector<WorkUnit>          m_workUnits;

// command interface
public:
	explicit AdaptiveSamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
