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
	OpenglFramebuffer();
	~OpenglFramebuffer() override;

	void bind() override;
	void unbind() override;
	void setAttachments(const GHIInfoFramebufferAttachment& attachments) override;
	void clearColor(uint32 slotIndex, const math::Vector4F& color) override;
	void clearDepthStencil(float32 depth, uint8 stencil) override;

private:
	void updateDeviceColorTexture(uint32 slotIndex, const OpenglFramebufferAttachmentInfo& newAttachment);
	void updateDeviceDepthStencilTexture(const OpenglFramebufferAttachmentInfo& newAttachment);

	OpenglFramebufferAttachmentInfo m_attachments;
	std::array<GLuint, GHIInfoFramebufferAttachment::MAX_COLOR_ATTACHMENTS> m_colorTextureIDs;
	GLuint m_depthStencilTextureID;
	GLuint m_framebufferID;
};

inline bool OpenglFramebufferFormat::isEmpty() const
{
	return internalFormat == 0;
}

}// end namespace ph::editor
