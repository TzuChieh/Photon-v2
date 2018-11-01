#include "Core/Renderer/PPM/PPMRenderer.h"

namespace ph
{

AttributeTags PPMRenderer::supportedAttributes() const
{

}

void PPMRenderer::init(const SdlResourcePack& data)
{

}

bool PPMRenderer::asyncSupplyWork(RenderWorker& worker)
{

}

void PPMRenderer::asyncSubmitWork(RenderWorker& worker)
{

}

ERegionStatus PPMRenderer::asyncPollUpdatedRegion(Region* out_region)
{

}

RenderStates PPMRenderer::asyncQueryRenderStates()
{

}

void PPMRenderer::asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute)
{

}

void PPMRenderer::develop(HdrRgbFrame& out_frame, EAttribute attribute)
{

}

// command interface

PPMRenderer::PPMRenderer(const InputPacket& packet)
{

}

SdlTypeInfo PPMRenderer::ciTypeInfo()
{

}

void PPMRenderer::ciRegister(CommandRegister& cmdRegister)
{

}

}// end namespace ph