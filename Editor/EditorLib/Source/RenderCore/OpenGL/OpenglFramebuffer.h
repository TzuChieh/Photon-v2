#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"
#include "RenderCore/OpenGL/opengl_enums.h"

#include "ThirdParty/glad2.h"

#include <Common/primitive_type.h>
#include <Math/TVector4.h>

#include <array>
#include <string>

namespace ph::editor
{

class OpenglTexture;

class OpenglColorAttachment final
{
public:
	GLenum internalFormat = GL_NONE;
	GHITextureHandle handle;

	bool isAttached() const
	{
		return !handle.isEmpty();
	}
};

class OpenglDepthStencilAttachment final
{
public:
	GLenum internalFormat = GL_NONE;
	GLenum attachmentType = GL_NONE;
	GHITextureHandle handle;

	bool isAttached() const
	{
		return !handle.isEmpty();
	}
};

class OpenglFramebuffer final
{
public:
	// Can actually allow more. Standard requires at least 8 (which should be enough for most use cases).
	inline static constexpr GLuint MAX_COLOR_ATTACHMENTS = 8;

	GLuint framebufferID = 0;
	GLsizei widthPx = 0;
	GLsizei heightPx = 0;
	GLsizei numSamples = 1;
	std::array<OpenglColorAttachment, MAX_COLOR_ATTACHMENTS> colorAttachments;
	OpenglDepthStencilAttachment depthStencilAttachment;

	void createBuffer(const GHIInfoFramebufferDesc& desc);

	void attachColor(
		uint32 attachmentIdx,
		const OpenglTexture& colorTexture, 
		GHITextureHandle handle);

	void attachDepthStencil(
		const OpenglTexture& depthStencilTexture,
		GHITextureHandle handle);
	
	void bind();
	void unbind();
	void clearColor(uint32 attachmentIdx, const math::Vector4F& color);
	void clearDepthStencil(float32 depth, uint8 stencil);
	void destroy();
	bool checkCompleteness();

	bool hasResource() const;

private:
	void createDeviceColorTexture(uint32 attachmentIndex);
	void createDeviceDepthStencilTexture();
	static std::string getFramebufferStatusInfo(GLuint framebufferID);
};

}// end namespace ph::editor
