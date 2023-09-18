#include "RenderCore/ghi_infos.h"

namespace ph::editor
{

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
