#include "RenderCore/OpenGL/OglTexture2D.h"

#include <Utility/utility.h>
#include <Common/assertion.h>

namespace ph::editor
{

OglTexture2D::OglTexture2D(const GLsizei widthPx, const GLsizei heightPx) :
	OglTexture2D(widthPx, heightPx, GL_LINEAR, GL_REPEAT)
{}

OglTexture2D::OglTexture2D(
	const GLsizei widthPx, 
	const GLsizei heightPx, 
	const GLenum  filterType, 
	const GLenum  clampType)
	: GHITexture2D({safe_number_cast<uint32>(widthPx), safe_number_cast<uint32>(heightPx)})
	, m_textureID(0)
{
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTextureStorage2D(m_textureID, 1, );// TODO

}

OglTexture2D::~OglTexture2D()
{
	// TODO
}

void OglTexture2D::loadPixels(void* const pixelData, const std::size_t numBytes)
{
	PH_ASSERT(pixelData);

	// TODO
}

std::size_t OglTexture2D::sizeInBytes() const
{
	std::size_t numBytesInSinglePixel = 0;
	switch(getFormat())
	{
	case EGHITextureFormat::RGB_8:  numBytesInSinglePixel = 3; break;
	case EGHITextureFormat::RGBA_8: numBytesInSinglePixel = 4; break;
	default: PH_ASSERT_UNREACHABLE_SECTION(); break;
	}

	return numBytesInSinglePixel * getSizePx().x() * getSizePx().y();
}

void* OglTexture2D::getNativeHandle()
{
	// TODO
}

GLenum OglTexture2D::toInternalFormat(const EGHITextureFormat format)
{
	switch(format)
	{
	case EGHITextureFormat::RGB_8:  return GL_RGB8;
	case EGHITextureFormat::RGBA_8: return GL_RGBA8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return GL_RGB8;
	}
}

}// end namespace ph::editor
