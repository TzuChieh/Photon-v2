#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"

#include "ThirdParty/glad2.h"

#include <Common/primitive_type.h>
#include <Math/math_fwd.h>
#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor::ghi
{

class OpenglTexture final
{
public:
	GLuint textureID = 0;
	GLsizei widthPx = 0;
	GLsizei heightPx = 0;
	GLenum internalFormat = GL_NONE;
	GLenum filterType = GL_LINEAR;
	GLenum wrapType = GL_REPEAT;
	GLsizei numSamples = 1;
	uint32 numPixelComponents : 3 = 0;
	uint32 isImmutableStorage : 1 = false;

	void create(const TextureDesc& desc);
	void createImmutableStorage(const TextureDesc& desc);
	void createMultiSampled(const TextureDesc& desc);

	void uploadPixelData(
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent);

	void uploadPixelData(
		const math::Vector3UI& newTextureSizePx,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent);

	void bindNonDSATexture() const;
	void bindSlot(uint32 slotIndex) const;
	void destroy();
	bool isColor() const;

	std::size_t numPixels() const
	{
		return static_cast<std::size_t>(widthPx) * heightPx;
	}

	bool isMultiSampled() const
	{
		return numSamples > 1;
	}

	bool hasResource() const
	{
		return textureID != 0;
	}
};

}// end namespace ph::editor::ghi
