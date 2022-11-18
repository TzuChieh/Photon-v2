#pragma once

#include "RenderCore/GHITexture2D.h"
#include "ThirdParty/glad2.h"
#include "RenderCore/OpenGL/opengl_states.h"

namespace ph::editor
{

class OpenglTexture2D : public GHITexture2D
{
public:
	OpenglTexture2D(
		GLsizei widthPx, 
		GLsizei heightPx, 
		GLenum internalFormat,
		const OpenglSampleState& sampleState);

	~OpenglTexture2D() override;

	void loadPixels(void* pixelData, std::size_t numBytes) override;
	void bind(uint32 slotIndex) override;
	MemoryInfo getMemoryInfo() const override;

	void* getNativeHandle() override;

private:
	std::size_t getApparentSize() const;

	GLuint m_textureID;
	GLsizei m_widthPx;
	GLsizei m_heightPx;
	GLenum m_internalFormat;
};

}// end namespace ph::editor
