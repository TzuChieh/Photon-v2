#include "RenderCore/GHIVertexStorage.h"

#include <Common/assertion.h>

namespace ph::editor
{

GHIInfoVertexAttributeLocator::GHIInfoVertexAttributeLocator()
	: strideOffset(0)
	, strideSize(0)
	, numElements(0)
	, shouldNormalize(false)
	, elementType(EGHIInfoStorageElement::Empty)
{}

std::size_t GHIInfoVertexAttributeLocator::numAttributeBytes() const
{
	return num_bytes(elementType) * numElements;
}

GHIInfoVertexGroupFormat::GHIInfoVertexGroupFormat()
	: attributes()
{}

std::size_t GHIInfoVertexGroupFormat::numGroupBytes() const
{
	std::size_t numBytes = 0;
	for(const GHIInfoVertexAttributeLocator& attribute : attributes)
	{
		numBytes += attribute.numAttributeBytes();
	}
	return numBytes;
}

GHIVertexStorage::GHIVertexStorage(const GHIInfoVertexGroupFormat& format, const EGHIInfoStorageUsage usage)
	: GHIStorage(usage)
	, m_format(format)
{}

GHIVertexStorage::~GHIVertexStorage() = default;

}// end namespace ph::editor
