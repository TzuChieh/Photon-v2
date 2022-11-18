#include "RenderCore/OpenGL/OpenglTexture2D.h"
#include "RenderCore/OpenGL/opengl_enums.h"

#include <Utility/utility.h>
#include <Common/assertion.h>

namespace ph::editor
{

/*
Using DSA functions--no need to bind for most situations.
https://www.khronos.org/opengl/wiki/Direct_State_Access
*/

OpenglTexture2D::OpenglTexture2D(
	const GLsizei widthPx,
	const GLsizei heightPx,
	const GLenum internalFormat,
	const OpenglSampleState& sampleState)

	: GHITexture2D({safe_number_cast<uint32>(widthPx), safe_number_cast<uint32>(heightPx)})

	, m_textureID(0)
	, m_widthPx(widthPx)
	, m_heightPx(heightPx)
	, m_internalFormat(internalFormat)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);

	// Cannot change size and format when created this way
	glTextureStorage2D(m_textureID, 1, internalFormat, widthPx, heightPx);
	
	glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, sampleState.filterType);
	glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, sampleState.filterType);

	glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, sampleState.wrapType);
	glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, sampleState.wrapType);
}

OpenglTexture2D::~OpenglTexture2D()
{
	glDeleteTextures(1, &m_textureID);
}

void OpenglTexture2D::loadPixels(void* const pixelData, const std::size_t numBytes)
{
	PH_ASSERT(pixelData);
	PH_ASSERT_EQ(numBytes, getApparentSize());

	glTextureSubImage2D(
		m_textureID, 0, 0, 0, m_widthPx, m_heightPx, m_internalFormat, GL_UNSIGNED_BYTE, pixelData);
}

void OpenglTexture2D::bind(const uint32 slotIndex)
{
	glBindTextureUnit(safe_number_cast<GLuint>(slotIndex), m_textureID);
}

auto OpenglTexture2D::getMemoryInfo() const
-> MemoryInfo
{
	MemoryInfo info{};
	info.sizePx.x() = m_widthPx;
	info.sizePx.y() = m_heightPx;
	info.apparentSize = getApparentSize();
	return info;
}

void* OpenglTexture2D::getNativeHandle()
{
	return &m_textureID;
}

std::size_t OpenglTexture2D::getApparentSize() const
{
	const auto ghiFormat = from_OpenGL_internal_format(m_internalFormat);
	return apparent_bytes_in_single_pixel(ghiFormat) * m_widthPx * m_heightPx;
}

}// end namespace ph::editor
