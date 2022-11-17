#pragma once

#include "RenderCore/GHITexture2D.h"
#include "ThirdParty/glad2.h"

namespace ph::editor
{

class OglTexture2D : public GHITexture2D
{
public:
	OglTexture2D(GLsizei widthPx, GLsizei heightPx);
	OglTexture2D(GLsizei widthPx, GLsizei heightPx, GLenum filterType, GLenum clampType);
	~OglTexture2D() override;

	void loadPixels(void* pixelData, std::size_t numBytes) override;
	std::size_t sizeInBytes() const override;

	void* getNativeHandle() override;

private:
	static GLenum toInternalFormat(EGHITextureFormat format);

	GLuint m_textureID;
};

}// end namespace ph::editor
