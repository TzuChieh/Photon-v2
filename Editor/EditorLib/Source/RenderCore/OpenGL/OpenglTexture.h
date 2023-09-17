#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"
#include "RenderCore/OpenGL/opengl_states.h"

#include "ThirdParty/glad2.h"

#include <Common/primitive_type.h>
#include <Math/math_fwd.h>
#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor
{

class OpenglTexture final
{
public:
	inline static constexpr GLuint DEFAULT_ID = 0;

	GLuint textureID = 0;
	GLsizei widthPx = 0;
	GLsizei heightPx = 0;
	GLenum internalFormat = GL_NONE;
	GLenum filterType = GL_LINEAR;
	GLenum wrapType = GL_REPEAT;
	uint32 numPixelComponents : 3 = 0;
	uint32 isImmutableStorage : 1 = false;

	void createImmutableStorage(const GHIInfoTextureDesc& desc);

	void uploadPixelData(
		TSpanView<std::byte> pixelData,
		EGHIPixelFormat pixelFormat,
		EGHIPixelComponent pixelComponent);

	void bind(uint32 slotIndex) const;

	void destroy();

	std::size_t numPixels() const;
	bool hasResource() const;
};

inline std::size_t OpenglTexture::numPixels() const
{
	return static_cast<std::size_t>(widthPx) * heightPx;
}

inline bool OpenglTexture::hasResource() const
{
	return textureID != DEFAULT_ID;
}

}// end namespace ph::editor
