#include "RenderCore/ghi_infos.h"

namespace ph::editor
{

bool GHIInfoTextureFormat::toFramebufferFormat(GHIInfoFramebufferFormat& framebufferFormat) const
{
	framebufferFormat.pixelFormat = pixelFormat;
	framebufferFormat.sampleState = sampleState;
	return true;
}

bool GHIInfoFramebufferFormat::toTextureFormat(GHIInfoTextureFormat& out_textureFormat) const
{
	out_textureFormat.pixelFormat = pixelFormat;
	out_textureFormat.sampleState = sampleState;
	return true;
}

std::size_t GHIInfoVertexAttributeLocator::numAttributeBytes() const
{
	return num_bytes(elementType) * numElements;
}

std::size_t GHIInfoVertexGroupFormat::numGroupBytes() const
{
	std::size_t numBytes = 0;
	for(const GHIInfoVertexAttributeLocator& attribute : attributes)
	{
		numBytes += attribute.numAttributeBytes();
	}
	return numBytes;
}

}// end namespace ph::editor
