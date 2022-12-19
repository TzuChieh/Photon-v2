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

OpenglTextureFormat::OpenglTextureFormat()
	: internalFormat(0)
	, sampleState()
	, numPixelComponents(0)
{}

OpenglTextureFormat::OpenglTextureFormat(const GHIInfoTextureFormat& format)
	: OpenglTextureFormat()
{
	internalFormat = opengl::to_internal_format(format.pixelFormat);
	sampleState = OpenglSampleState(format.sampleState);
	numPixelComponents = opengl::num_pixel_components(internalFormat);
}

OpenglTexture2D::OpenglTexture2D(
	const GHIInfoTextureFormat& format,
	const math::Vector2UI& sizePx)

	: GHITexture2D(format)

	, m_textureID(0)
	, m_widthPx(safe_integer_cast<GLsizei>(sizePx.x()))
	, m_heightPx(safe_integer_cast<GLsizei>(sizePx.y()))
	, m_format(format)
{
	// Currently depth is not supported
	PH_ASSERT(opengl::is_color_format(m_format.internalFormat));

	glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);

	// Using immutable texture object, cannot change size and format when created this way
	glTextureStorage2D(m_textureID, 1, m_format.internalFormat, m_widthPx, m_heightPx);
	
	glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, m_format.sampleState.filterType);
	glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, m_format.sampleState.filterType);

	glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, m_format.sampleState.wrapType);
	glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, m_format.sampleState.wrapType);
}

OpenglTexture2D::~OpenglTexture2D()
{
	glDeleteTextures(1, &m_textureID);
}

void OpenglTexture2D::upload(
	const std::byte* const pixelData,
	const std::size_t numBytes,
	const EGHIInfoPixelComponent componentType)
{
	PH_ASSERT(pixelData);

	// The input pixel data must be for the entire texture--same number of total pixel components
	PH_ASSERT_EQ(
		numPixels() * m_format.numPixelComponents, 
		numBytes / num_bytes(componentType));

	// Format of input pixel data must be compatible to the internal format
	const GLenum pixelDataFormat = opengl::to_base_format(m_format.internalFormat);
	
	// Type of each pixel component in the input pixel data
	GLenum pixelComponentType = 0;
	switch(componentType)
	{
	case EGHIInfoPixelComponent::UInt8:
		pixelComponentType = GL_UNSIGNED_BYTE;
		break;

	case EGHIInfoPixelComponent::Float16:
		pixelComponentType = GL_HALF_FLOAT;
		break;

	case EGHIInfoPixelComponent::Float32:
		pixelComponentType = GL_FLOAT;
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}

	glTextureSubImage2D(
		m_textureID, 0, 0, 0, m_widthPx, m_heightPx, pixelDataFormat, pixelComponentType, pixelData);
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
	info.apparentSize = numApparentSizeInBytes();
	return info;
}

auto OpenglTexture2D::getNativeHandle()
-> NativeHandle
{
	if(m_textureID != 0)
	{
		return safe_number_cast<uint64>(m_textureID);
	}
	else
	{
		return std::monostate{};
	}
}

std::size_t OpenglTexture2D::numApparentSizeInBytes() const
{
	const auto ghiFormat = opengl::from_internal_format(m_format.internalFormat);
	return num_bytes(ghiFormat) * numPixels();
}

std::size_t OpenglTexture2D::numPixels() const
{
	return static_cast<std::size_t>(m_widthPx) * m_heightPx;
}

}// end namespace ph::editor
