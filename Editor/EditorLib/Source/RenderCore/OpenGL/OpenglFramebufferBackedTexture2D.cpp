#include "RenderCore/OpenGL/OpenglFramebufferBackedTexture2D.h"
#include "RenderCore/OpenGL/opengl_enums.h"
#include "RenderCore/OpenGL/OpenglFramebuffer.h"

#include <Math/TVector2.h>
#include <Math/TVector2.h>

namespace ph::editor
{

OpenglFramebufferBackedTexture2D::OpenglFramebufferBackedTexture2D(
	const std::shared_ptr<OpenglFramebuffer>& framebuffer,
	const GLuint textureID,
	const uint32 attachmentIndex,
	const bool isDepthStencilAttachment)

	: GHITexture2D()

	, m_framebuffer(framebuffer)
	, m_attachmentIndex(attachmentIndex)
	, m_isDepthStencilAttachment(isDepthStencilAttachment)
	, m_textureID(textureID)
{}

OpenglFramebufferBackedTexture2D::~OpenglFramebufferBackedTexture2D()
{
	// Release nothing, the texture resource is owned by framebuffer
}

void OpenglFramebufferBackedTexture2D::upload(
	const std::byte* const pixelData,
	const std::size_t numBytes,
	const EGHIInfoPixelComponent componentType)
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

void OpenglFramebufferBackedTexture2D::bind(const uint32 slotIndex)
{
	glBindTextureUnit(safe_number_cast<GLuint>(slotIndex), m_textureID);
}

auto OpenglFramebufferBackedTexture2D::getMemoryInfo() const
-> MemoryInfo
{
	MemoryInfo info{};
	info.sizePx.x() = getSizePx().x();
	info.sizePx.y() = getSizePx().y();
	info.apparentSize = numApparentSizeInBytes();
	return info;
}

auto OpenglFramebufferBackedTexture2D::getNativeHandle()
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

std::size_t OpenglFramebufferBackedTexture2D::numApparentSizeInBytes() const
{
	const auto ghiFormat = opengl::from_internal_format(getInternalFormat());
	return num_bytes(ghiFormat) * numPixels();
}

std::size_t OpenglFramebufferBackedTexture2D::numPixels() const
{
	return getSizePx().product();
}

GLenum OpenglFramebufferBackedTexture2D::getInternalFormat() const
{
	GLenum internalFormat = 0;
	if(m_framebuffer)
	{
		if(!m_isDepthStencilAttachment)
		{
			internalFormat = m_framebuffer->getAttachments().colorFormats[m_attachmentIndex].internalFormat;
		}
		else
		{
			internalFormat = m_framebuffer->getAttachments().depthStencilFormat.internalFormat;
		}
	}

	return internalFormat;
}

math::Vector2S OpenglFramebufferBackedTexture2D::getSizePx() const
{
	math::Vector2S sizePx(0);
	if(m_framebuffer)
	{
		sizePx.x() = m_framebuffer->getAttachments().widthPx;
		sizePx.y() = m_framebuffer->getAttachments().heightPx;
	}

	return sizePx;
}

}// end ph::editor
