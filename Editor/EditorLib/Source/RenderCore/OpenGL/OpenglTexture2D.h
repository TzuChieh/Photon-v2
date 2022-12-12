#pragma once

#include "RenderCore/GHITexture2D.h"
#include "ThirdParty/glad2.h"
#include "RenderCore/OpenGL/opengl_states.h"

#include <Common/primitive_type.h>

namespace ph::editor
{

class OpenglTextureFormat final
{
public:
	GLenum internalFormat;
	OpenglSampleState sampleState;
	uint32 numPixelComponents;

	OpenglTextureFormat();
	explicit OpenglTextureFormat(const GHIInfoTextureFormat& format);
};

class OpenglTexture2D : public GHITexture2D
{
public:
	OpenglTexture2D(
		const OpenglTextureFormat& format,
		GLsizei widthPx, 
		GLsizei heightPx);

	~OpenglTexture2D() override;

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

	GLuint m_textureID;
	GLsizei m_widthPx;
	GLsizei m_heightPx;
	OpenglTextureFormat m_format;
};

}// end namespace ph::editor
