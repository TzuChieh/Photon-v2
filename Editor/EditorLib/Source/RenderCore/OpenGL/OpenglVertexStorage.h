#pragma once

#include "RenderCore/GHIVertexStorage.h"
#include "ThirdParty/glad2.h"

namespace ph::editor
{

class OpenglVertexAttributeLocator final
{
public:
	/*! Number of bytes to offset from the start of the vertex buffer. */
	GLintptr bufferOffset;

	/*! Number of bytes to offset from the start of the vertex. Effectively added to `bufferOffset` to
	obtain the final offset for the attribute.
	*/
	GLuint relativeOffset;

	GLenum elementType;
	GLint numElements;
	GLboolean shouldNormalize;

	/*! @brief Empty attribute.
	*/
	OpenglVertexAttributeLocator();

	bool isEmpty() const;
};

class OpenglVertexLayout final
{
public:
	std::array<OpenglVertexAttributeLocator, GHIInfoVertexLayout::MAX_VERTEX_ATTRIBUTES> attributes;

	explicit OpenglVertexLayout(const GHIInfoVertexLayout& vertexLayout);
};

class OpenglVertexStorage : public GHIVertexStorage
{
public:
	explicit OpenglVertexStorage(const GHIInfoVertexLayout& vertexLayout);

	virtual void upload(
		const std::byte* vertexData,
		std::size_t numBytes,
		EGHIInfoStorageElement elementType) = 0;

	const OpenglVertexLayout& getOpenglVertexLayout() const;

private:
	OpenglVertexLayout m_vertexLayout;
};

inline bool OpenglVertexAttributeLocator::isEmpty() const
{
	return elementType == GL_NONE;
}

inline const OpenglVertexLayout& OpenglVertexStorage::getOpenglVertexLayout() const
{
	return m_vertexLayout;
}

}// end namespace ph::editor
