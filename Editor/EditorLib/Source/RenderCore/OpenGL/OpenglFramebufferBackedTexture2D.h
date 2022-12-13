#pragma once

#include "RenderCore/GHITexture2D.h"
#include "ThirdParty/glad2.h"

#include <Common/primitive_type.h>
#include <Math/math_fwd.h>

#include <memory>

namespace ph::editor
{

class OpenglFramebuffer;

class OpenglFramebufferBackedTexture2D : public GHITexture2D
{
public:
	OpenglFramebufferBackedTexture2D(
		const std::shared_ptr<OpenglFramebuffer>& framebuffer,
		GLuint textureID,
		uint32 attachmentIndex,
		bool isDepthStencilAttachment = false);

	~OpenglFramebufferBackedTexture2D() override;

	void upload(
		const std::byte* pixelData,
		std::size_t numBytes,
		EGHIInfoPixelComponent componentType) override;

	void bind(uint32 slotIndex) override;
	MemoryInfo getMemoryInfo() const override;
	NativeHandle getNativeHandle() override;

private:
	std::size_t numApparentSizeInBytes() const;
	std::size_t numPixels() const;
	GLenum getInternalFormat() const;
	math::Vector2S getSizePx() const;

	std::shared_ptr<OpenglFramebuffer> m_framebuffer;
	uint32 m_attachmentIndex : 8;
	uint32 m_isDepthStencilAttachment : 1;
	GLuint m_textureID;
};

}// end namespace ph::editor
