#include "RenderCore/OpenGL/OpenglTexture.h"
#include "RenderCore/OpenGL/opengl_enums.h"
#include "RenderCore/ghi_infos.h"

#include <Utility/utility.h>
#include <Common/assertion.h>
#include <Common/logging.h>
#include <Math/TVector3.h>

namespace ph::editor
{

/*
Using DSA functions--no need to bind for most situations.
https://www.khronos.org/opengl/wiki/Direct_State_Access
*/

void OpenglTexture::create(const GHIInfoTextureDesc& desc)
{
	if(desc.format.numSamples != 1)
	{
		createMultiSampled(desc);
	}
	else
	{
		createImmutableStorage(desc);
	}
}

void OpenglTexture::createImmutableStorage(const GHIInfoTextureDesc& desc)
{
	PH_ASSERT(!hasResource());
	PH_ASSERT_EQ(desc.format.numSamples, 1);

	widthPx = lossless_cast<GLsizei>(desc.sizePx.x());
	heightPx = lossless_cast<GLsizei>(desc.sizePx.y());
	internalFormat = opengl::to_internal_format(desc.format.pixelFormat);
	filterType = opengl::to_filter_type(desc.format.sampleState.filterMode);
	wrapType = opengl::to_wrap_type(desc.format.sampleState.wrapMode);
	numPixelComponents = opengl::num_pixel_components(internalFormat);
	isImmutableStorage = true;

	// Currently depth is not supported
	PH_ASSERT(opengl::is_color_format(internalFormat));

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

	// Using immutable texture object, cannot change size and format when created this way
	glTextureStorage2D(textureID, 1, internalFormat, widthPx, heightPx);
	
	glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, filterType);
	glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, filterType);

	glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, wrapType);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, wrapType);
}

void OpenglTexture::createMultiSampled(const GHIInfoTextureDesc& desc)
{
	PH_ASSERT(!hasResource());
	PH_ASSERT_GT(desc.format.numSamples, 1);

	widthPx = lossless_cast<GLsizei>(desc.sizePx.x());
	heightPx = lossless_cast<GLsizei>(desc.sizePx.y());
	internalFormat = opengl::to_internal_format(desc.format.pixelFormat);
	numPixelComponents = opengl::num_pixel_components(internalFormat);
	numSamples = lossless_cast<GLsizei>(desc.format.numSamples);
	isImmutableStorage = false;

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

	// Need bind since we are using non-DSA calls here (for mutable textures)
	bindNonDSATexture();

	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE,
		numSamples,
		internalFormat,
		widthPx,
		heightPx,
		GL_FALSE);
}

void OpenglTexture::uploadPixelData(
	TSpanView<std::byte> pixelData,
	EGHIPixelFormat pixelFormat,
	EGHIPixelComponent pixelComponent)
{
	PH_ASSERT(hasResource());
	PH_ASSERT(pixelData.data());

	if(isMultiSampled())
	{
		PH_DEFAULT_LOG_WARNING(
			"Cannot upload pixel data from host to a multi-sampled texture.");
		return;
	}

	// TODO: format of input pixel data should be compatible to the internal format
	GLenum unsizedFormat = opengl::to_internal_format(pixelFormat);

	glTextureSubImage2D(
		textureID, 
		0, 
		0, 
		0, 
		widthPx, 
		heightPx, 
		unsizedFormat,// meaning of each pixel component in `pixelData`
		opengl::to_data_type(pixelComponent),// type of each pixel component in `pixelData`
		pixelData.data());
}

void OpenglTexture::uploadPixelData(
	const math::Vector3UI& newTextureSizePx,
	TSpanView<std::byte> pixelData,
	EGHIPixelFormat pixelFormat,
	EGHIPixelComponent pixelComponent)
{
	PH_ASSERT(hasResource());
	PH_ASSERT(pixelData.data());

	if(isMultiSampled())
	{
		PH_DEFAULT_LOG_WARNING(
			"Cannot upload pixel data from host to a multi-sampled texture.");
		return;
	}

	// Need bind since we are using non-DSA calls here (for mutable textures)
	bindNonDSATexture();

	// TODO: format of input pixel data should be compatible to the internal format
	GLenum unsizedPixelFormat = opengl::to_internal_format(pixelFormat);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalFormat,// We do not allow changing internal format currently
		lossless_cast<GLsizei>(newTextureSizePx.x()),
		lossless_cast<GLsizei>(newTextureSizePx.y()),
		0,
		unsizedPixelFormat,// meaning of each pixel component in `pixelData`
		opengl::to_data_type(pixelComponent),// type of each pixel component in `pixelData`
		pixelData.data());
}

void OpenglTexture::bindNonDSATexture() const
{
	if(isMultiSampled())
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
	}
}

void OpenglTexture::bindSlot(const uint32 slotIndex) const
{
	glBindTextureUnit(lossless_integer_cast<GLuint>(slotIndex), textureID);
}

void OpenglTexture::destroy()
{
	glDeleteTextures(1, &textureID);
	textureID = 0;
}

bool OpenglTexture::isColor() const
{
	return opengl::is_color_format(internalFormat);
}

}// end namespace ph::editor
