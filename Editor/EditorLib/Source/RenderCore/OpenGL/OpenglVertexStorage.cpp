#include "RenderCore/OpenGL/OpenglVertexStorage.h"

#include <Utility/utility.h>

namespace ph::editor
{

OpenglVertexAttributeLocator::OpenglVertexAttributeLocator()
	: bufferOffset(0)
	, relativeOffset(0)
	, elementType(GL_NONE)
	, numElements(0)
	, shouldNormalize(GL_FALSE)
{}

OpenglVertexLayout::OpenglVertexLayout(const GHIInfoVertexLayout& vertexLayout)
	: attributes()
{
	for(std::size_t attribIdx = 0; attribIdx < vertexLayout.attributes.size(); ++attribIdx)
	{
		const GHIInfoVertexAttributeLocator& ghiAttribute = vertexLayout.attributes[attribIdx];
		if(ghiAttribute.isEmpty())
		{
			continue;
		}

		OpenglVertexAttributeLocator& attribute = attributes[attribIdx];
		lossless_cast(ghiAttribute.strideOffset, &attribute.bufferOffset);
		lossless_cast(ghiAttribute.strideSize, &attribute.relativeOffset);




	}


}

OpenglVertexStorage::OpenglVertexStorage(const GHIInfoVertexLayout& vertexLayout)
	: GHIVertexStorage(vertexLayout)
	, m_vertexLayout(vertexLayout)
{
	bool hasSameStrideOffset = true;
	for(std::size_t attribIdx = 1; attribIdx < vertexLayout.attributes.size(); ++attribIdx)
	{
		const auto currStrideOffset = vertexLayout.attributes[attribIdx].strideOffset;
		const auto prevStrideOffset = vertexLayout.attributes[attribIdx - 1].strideOffset;
		if(currStrideOffset != prevStrideOffset)
		{
			hasSameStrideOffset = false;
			break;
		}
	}
}

}// end namespace ph::editor
