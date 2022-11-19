#include "RenderCore/OpenGL/opengl_enums.h"

#include <Common/assertion.h>

namespace ph::editor
{

GLenum to_OpenGL_internal_format(const EGHITextureFormat format)
{
	switch(format)
	{
	case EGHITextureFormat::Empty: return 0;
	case EGHITextureFormat::RGB_8: return GL_RGB8;
	case EGHITextureFormat::RGBA_8: return GL_RGBA8;
	case EGHITextureFormat::RGB_16F: return GL_RGB16F;
	case EGHITextureFormat::RGBA_16F: return GL_RGBA16F;
	case EGHITextureFormat::RGB_32F: return GL_RGB32F;
	case EGHITextureFormat::RGBA_32F: return GL_RGBA32F;
	case EGHITextureFormat::Depth_24_Stencil_8: return GL_DEPTH24_STENCIL8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

EGHITextureFormat from_OpenGL_internal_format(const GLenum internalFormat)
{
	switch(internalFormat)
	{
	case 0: return EGHITextureFormat::Empty;
	case GL_RGB8: return EGHITextureFormat::RGB_8;
	case GL_RGBA8: return EGHITextureFormat::RGBA_8;
	case GL_RGB16F: return EGHITextureFormat::RGB_16F;
	case GL_RGBA16F: return EGHITextureFormat::RGBA_16F;
	case GL_RGB32F: return EGHITextureFormat::RGB_32F;
	case GL_RGBA32F: return EGHITextureFormat::RGBA_32F;
	case GL_DEPTH24_STENCIL8: return EGHITextureFormat::Depth_24_Stencil_8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return EGHITextureFormat::Empty;
	}
}

}// end namespace ph::editor
