#include "RenderCore/OpenGL/OpenglFramebuffer.h"
#include "RenderCore/OpenGL/opengl_enums.h"

#include <Utility/utility.h>

namespace ph::editor
{

OpenglFramebufferFormat::OpenglFramebufferFormat()
	: internalFormat(0)
	, sampleState()
{}

OpenglFramebufferFormat::OpenglFramebufferFormat(const GHIFramebufferFormat& format)
	: OpenglFramebufferFormat()
{
	internalFormat = to_OpenGL_internal_format(format.textureFormat);
	sampleState = OpenglSampleState(format.sampleState);
}

OpenglFramebufferAttachmentInfo::OpenglFramebufferAttachmentInfo()
	: widthPx(0)
	, heightPx(0)
	, numSamples(1)
	, colorFormats()
	, depthStencilFormat()
{}

OpenglFramebufferAttachmentInfo::OpenglFramebufferAttachmentInfo(const GHIFramebufferAttachmentInfo& attachments)
	: OpenglFramebufferAttachmentInfo()
{
	widthPx = safe_number_cast<GLsizei>(attachments.sizePx.x());
	heightPx = safe_number_cast<GLsizei>(attachments.sizePx.y());
	numSamples = safe_number_cast<GLsizei>(attachments.numSamples);

	for(std::size_t i = 0; i < attachments.colorFormats.size(); ++i)
	{
		colorFormats[i] = OpenglFramebufferFormat(attachments.colorFormats[i]);
	}

	depthStencilFormat = OpenglFramebufferFormat(attachments.depthStencilFormat);
}

OpenglFramebuffer::OpenglFramebuffer()
	: GHIFramebuffer()
	, m_attachments()
	, m_textureIDs{}
	, m_depthStencilTextureID(0)
{

}

OpenglFramebuffer::~OpenglFramebuffer()
{
	// TODO
}

void OpenglFramebuffer::bind()
{

}

void OpenglFramebuffer::unbind()
{

}

void OpenglFramebuffer::setAttachments(const GHIFramebufferAttachmentInfo& attachments)
{
	const OpenglFramebufferAttachmentInfo newAttachments(attachments);

}

void OpenglFramebuffer::clearColor(uint32 slotIndex, const math::Vector4F& color)
{

}

void OpenglFramebuffer::clearDepthStencil()
{

}

}// end namespace ph::editor
