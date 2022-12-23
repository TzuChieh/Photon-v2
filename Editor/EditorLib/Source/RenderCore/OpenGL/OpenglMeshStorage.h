#pragma once

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

}// end namespace ph::editor
