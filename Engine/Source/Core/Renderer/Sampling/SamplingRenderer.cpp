#include "Core/Renderer/Sampling/SamplingRenderer.h"

namespace ph
{

// command interface

SamplingRenderer::SamplingRenderer(const InputPacket& packet) :
	Renderer(packet)
{}

SdlTypeInfo SamplingRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "sampling");
}

void SamplingRenderer::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph