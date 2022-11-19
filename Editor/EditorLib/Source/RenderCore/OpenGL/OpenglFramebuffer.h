#pragma once

#include "RenderCore/GHIFramebuffer.h"
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
	explicit OpenglFramebufferFormat(const GHIFramebufferFormat& format);

	bool isEmpty() const;
	inline bool operator == (const OpenglFramebufferFormat& rhs) const = default;
};

class OpenglFramebufferAttachmentInfo final
{
public:
	GLsizei widthPx;
	GLsizei heightPx;
	GLsizei numSamples;
	std::array<OpenglFramebufferFormat, GHIFramebufferAttachmentInfo::MAX_COLOR_ATTACHMENTS> colorFormats;
	OpenglFramebufferFormat depthStencilFormat;

	OpenglFramebufferAttachmentInfo();
	explicit OpenglFramebufferAttachmentInfo(const GHIFramebufferAttachmentInfo& attachments);
};

class OpenglFramebuffer : public GHIFramebuffer
{
public:
	OpenglFramebuffer();
	~OpenglFramebuffer() override;

	void bind() override;
	void unbind() override;
	void setAttachments(const GHIFramebufferAttachmentInfo& attachments) override;
	void clearColor(uint32 slotIndex, const math::Vector4F& color) override;
	void clearDepthStencil() override;

private:
	void allocateDeviceColorTexture();

	OpenglFramebufferAttachmentInfo m_attachments;
	std::array<GLuint, GHIFramebufferAttachmentInfo::MAX_COLOR_ATTACHMENTS> m_textureIDs;
	GLuint m_depthStencilTextureID;
	GLuint m_framebufferID;
};

inline bool OpenglFramebufferFormat::isEmpty() const
{
	return internalFormat == 0;
}

}// end namespace ph::editor
