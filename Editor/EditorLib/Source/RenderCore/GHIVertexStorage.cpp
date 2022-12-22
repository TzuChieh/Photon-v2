#include "RenderCore/GHIVertexStorage.h"

#include <Common/assertion.h>

namespace ph::editor
{

GHIInfoVertexAttributeLocator::GHIInfoVertexAttributeLocator()
	: inGroupOffset(0)
	, numElements(0)
	, shouldNormalize(false)
	, elementType(EGHIInfoStorageElement::Empty)
{}

std::size_t GHIInfoVertexAttributeLocator::numAttributeBytes() const
{
	return num_bytes(elementType) * numElements;
}

GHIInfoVertexGroup::GHIInfoVertexGroup()
	: groupOffset(0)
	, attributeIndices()
	, numAttributes(0)
{}

GHIInfoVertexLayout::GHIInfoVertexLayout()
	: attributes()
	, groups()
{}

std::size_t GHIInfoVertexLayout::numGroupBytes(const std::size_t groupIndex) const
{
	PH_ASSERT_LT(groupIndex, groups.size());
	const GHIInfoVertexGroup& group = groups[groupIndex];

	std::size_t numBytes = 0;
	for(uint8 ai = 0; ai < group.numAttributes; ++ai)
	{
		const GHIInfoVertexAttributeLocator& attribute = attributes[ai];
		PH_ASSERT(!attribute.isEmpty());

		numBytes += attribute.numAttributeBytes();
	}
	return numBytes;
}

std::size_t GHIInfoVertexLayout::numVertexBytes() const
{
	std::size_t numBytes = 0;
	for(const GHIInfoVertexAttributeLocator& attribute : attributes)
	{
		numBytes += attribute.numAttributeBytes();
	}
	return numBytes;
}

GHIVertexStorage::GHIVertexStorage(const GHIInfoVertexLayout& vertexLayout)
	: GHIStorage()
	, m_vertexLayout(vertexLayout)
{}

GHIVertexStorage::~GHIVertexStorage() = default;

}// end namespace ph::editor
