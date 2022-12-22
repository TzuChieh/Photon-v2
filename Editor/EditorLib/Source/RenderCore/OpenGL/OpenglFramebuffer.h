#pragma once

#include "RenderCore/GHIFramebuffer.h"
#include "RenderCore/OpenGL/opengl_enums.h"
#include "RenderCore/OpenGL/opengl_states.h"

#include <array>

namespace ph::editor
{

class OpenglFramebufferFormat final
{
public:
	GLenum internalFormat;
	OpenglSampleState sampleState;

	OpenglFramebufferFormat();
	explicit OpenglFramebufferFormat(const GHIInfoFramebufferFormat& format);

	bool isEmpty() const;
	inline bool operator == (const OpenglFramebufferFormat& rhs) const = default;
};

class OpenglFramebufferAttachmentInfo final
{
public:
	GLsizei widthPx;
	GLsizei heightPx;
	GLsizei numSamples;
	std::array<OpenglFramebufferFormat, GHIInfoFramebufferAttachment::MAX_COLOR_ATTACHMENTS> colorFormats;
	OpenglFramebufferFormat depthStencilFormat;
	GLenum depthStencilAttachment;

	OpenglFramebufferAttachmentInfo();
	explicit OpenglFramebufferAttachmentInfo(const GHIInfoFramebufferAttachment& attachments);
};

class OpenglFramebuffer : public GHIFramebuffer
{
public:
	explicit OpenglFramebuffer(const GHIInfoFramebufferAttachment& attachments);
	~OpenglFramebuffer() override;

	void bind() override;
	void unbind() override;
	void clearColor(uint32 attachmentIndex, const math::Vector4F& color) override;
	void clearDepthStencil(float32 depth, uint8 stencil) override;
	std::shared_ptr<GHITexture2D> createTextureFromColor(uint32 slotIndex) override;

	const OpenglFramebufferAttachmentInfo& getOpenglAttachments() const;

private:
	void createDeviceColorTexture(uint32 attachmentIndex);
	void createDeviceDepthStencilTexture();

	OpenglFramebufferAttachmentInfo m_attachments;
	std::array<GLuint, GHIInfoFramebufferAttachment::MAX_COLOR_ATTACHMENTS> m_colorTextureIDs;
	GLuint m_depthStencilTextureID;
	GLuint m_framebufferID;
};

inline bool OpenglFramebufferFormat::isEmpty() const
{
	return internalFormat == GL_NONE;
}

inline const OpenglFramebufferAttachmentInfo& OpenglFramebuffer::getOpenglAttachments() const
{
	return m_attachments;
}

}// end namespace ph::editor
