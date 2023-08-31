#include "RenderCore/OpenGL/OpenglTexture.h"
#include "RenderCore/OpenGL/opengl_enums.h"
#include "RenderCore/ghi_infos.h"

#include <Utility/utility.h>
#include <Common/assertion.h>
#include <Math/TVector3.h>

namespace ph::editor
{

/*
Using DSA functions--no need to bind for most situations.
https://www.khronos.org/opengl/wiki/Direct_State_Access
*/



//
//
//auto OpenglTexture2D::getNativeHandle()
//-> NativeHandle
//{
//	if(m_textureID != 0)
//	{
//		return static_cast<uint64>(m_textureID);
//	}
//	else
//	{
//		return std::monostate{};
//	}
//}
//

void OpenglTexture::createImmutableStorage(const GHIInfoTextureDesc& desc)
{
	PH_ASSERT(!hasResource());

	widthPx = lossless_integer_cast<GLsizei>(desc.sizePx.x());
	heightPx = lossless_integer_cast<GLsizei>(desc.sizePx.y());
	internalFormat = opengl::to_internal_format(desc.format.pixelFormat);
	filterType = opengl::translate(desc.format.sampleState.filterMode);
	wrapType = opengl::translate(desc.format.sampleState.wrapMode);
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

void OpenglTexture::uploadPixelData(
	TSpanView<std::byte> pixelData,
	const EGHIPixelComponent componentType)
{
	PH_ASSERT(hasResource());
	PH_ASSERT(pixelData.data());

	// The input pixel data must be for the entire texture--same number of total pixel components
	PH_ASSERT_EQ(
		numPixels() * numPixelComponents, 
		pixelData.size() / num_bytes(componentType));

	// Format of input pixel data must be compatible to the internal format
	const GLenum pixelDataFormat = opengl::to_base_format(internalFormat);

	glTextureSubImage2D(
		textureID, 
		0, 
		0, 
		0, 
		widthPx, 
		heightPx, 
		pixelDataFormat, 
		opengl::translate(componentType),// type of each pixel component in the raw pixel data input
		pixelData.data());
}

void OpenglTexture::bind(const uint32 slotIndex) const
{
	glBindTextureUnit(lossless_integer_cast<GLuint>(slotIndex), textureID);
}

void OpenglTexture::destroy()
{
	glDeleteTextures(1, &textureID);
	textureID = DEFAULT_ID;
}

}// end namespace ph::editor
