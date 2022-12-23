#include "RenderCore/OpenGL/OpenglVertexStorage.h"

#include <Utility/utility.h>
#include <Common/logging.h>
#include <Common/assertion.h>

namespace ph::editor
{

//OpenglVertexAttributeLocator::OpenglVertexAttributeLocator()
//	: bufferOffset(0)
//	, relativeOffset(0)
//	, elementType(GL_NONE)
//	, numElements(0)
//	, shouldNormalize(GL_FALSE)
//{}
//
//OpenglVertexLayout::OpenglVertexLayout(const GHIInfoVertexLayout& vertexLayout)
//	: attributes()
//{
//	for(std::size_t attribIdx = 0; attribIdx < vertexLayout.attributes.size(); ++attribIdx)
//	{
//		const GHIInfoVertexAttributeLocator& ghiAttribute = vertexLayout.attributes[attribIdx];
//		if(ghiAttribute.isEmpty())
//		{
//			continue;
//		}
//
//		OpenglVertexAttributeLocator& attribute = attributes[attribIdx];
//		lossless_cast(ghiAttribute.strideOffset, &attribute.bufferOffset);
//		lossless_cast(ghiAttribute.strideSize, &attribute.relativeOffset);
//
//
//
//
//	}
//
//
//}

OpenglVertexStorage::OpenglVertexStorage(
	const GHIInfoVertexGroupFormat& format,
	const std::size_t numVertices,
	const EGHIInfoStorageUsage usage)

	: GHIVertexStorage(format, usage)

	, m_vboID(0)
	, m_numVertices(numVertices)
{
	glCreateBuffers(1, &m_vboID);
}

OpenglVertexStorage::~OpenglVertexStorage()
{
	glDeleteBuffers(1, &m_vboID);
}

void OpenglVertexStorage::upload(
	const std::byte* const vertexData,
	const std::size_t inNumBytes)
{
	PH_ASSERT(vertexData);
	PH_ASSERT_NE(m_vboID, 0);

	// The input data must be for the entire vertex buffer--same number of total bytes
	PH_ASSERT_EQ(numBytes(), inNumBytes);

	if(getUsage() == EGHIInfoStorageUsage::Static)
	{
		glNamedBufferStorage(
			m_vboID,
			lossless_cast<GLsizeiptr>(numBytes()),
			vertexData,
			0);
	}
	else
	{
		glNamedBufferData(
			m_vboID,
			lossless_cast<GLsizeiptr>(numBytes()),
			vertexData,
			GL_DYNAMIC_DRAW);
	}
}

auto OpenglVertexStorage::getNativeHandle()
-> NativeHandle
{
	if(m_vboID != 0)
	{
		return static_cast<uint64>(m_vboID);
	}
	else
	{
		return std::monostate{};
	}
}

std::size_t OpenglVertexStorage::numBytes() const
{
	return getFormat().numGroupBytes() * m_numVertices;
}

}// end namespace ph::editor
