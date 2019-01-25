#pragma once

#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Core/Renderer/Region/WorkScheduler.h"
#include "Core/Renderer/Region/WorkVolume.h"

#include <memory>
#include <vector>

namespace ph
{

class EqualSamplingRenderer : public SamplingRenderer, public TCommandInterface<EqualSamplingRenderer>
{
public:
	void doUpdate(const SdlResourcePack& data) override;
	bool supplyWork(uint32 workerId, SamplingRenderWork& work) override;
	void submitWork(uint32 workerId, SamplingRenderWork& work) override;

private:
	std::unique_ptr<WorkScheduler> m_workScheduler;
	std::vector<WorkVolume>        m_workVolumes;

// command interface
public:
	explicit EqualSamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
