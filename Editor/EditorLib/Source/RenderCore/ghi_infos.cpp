#include "RenderCore/ghi_infos.h"

namespace ph::editor::ghi
{

std::size_t VertexAttributeLocatorInfo::numAttributeBytes() const
{
	return num_bytes(elementType) * numElements;
}

std::size_t VertexGroupFormatInfo::numGroupBytes() const
{
	std::size_t numBytes = 0;
	for(const VertexAttributeLocatorInfo& attribute : attributes)
	{
		numBytes += attribute.numAttributeBytes();
	}
	return numBytes;
}

}// end namespace ph::editor::ghi
