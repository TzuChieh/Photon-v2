#include "RenderCore/OpenGL/opengl_enums.h"

#include <Common/assertion.h>

namespace ph::editor::opengl
{

GLenum to_internal_format(const EGHIInfoPixelFormat format)
{
	switch(format)
	{
	case EGHIInfoPixelFormat::Empty: return GL_NONE;
	case EGHIInfoPixelFormat::RGB_8: return GL_RGB8;
	case EGHIInfoPixelFormat::RGBA_8: return GL_RGBA8;
	case EGHIInfoPixelFormat::RGB_16F: return GL_RGB16F;
	case EGHIInfoPixelFormat::RGBA_16F: return GL_RGBA16F;
	case EGHIInfoPixelFormat::RGB_32F: return GL_RGB32F;
	case EGHIInfoPixelFormat::RGBA_32F: return GL_RGBA32F;
	case EGHIInfoPixelFormat::Depth_24_Stencil_8: return GL_DEPTH24_STENCIL8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return GL_NONE;
	}
}

GLenum to_color_attachment(const uint32 slotIndex)
{
	switch(slotIndex)
	{
	case 0: return GL_COLOR_ATTACHMENT0;
	case 1: return GL_COLOR_ATTACHMENT1;
	case 2: return GL_COLOR_ATTACHMENT2;
	case 3: return GL_COLOR_ATTACHMENT3;
	case 4: return GL_COLOR_ATTACHMENT4;
	case 5: return GL_COLOR_ATTACHMENT5;
	case 6: return GL_COLOR_ATTACHMENT6;
	case 7: return GL_COLOR_ATTACHMENT7;
	case 8: return GL_COLOR_ATTACHMENT8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return GL_NONE;
	}
}

GLenum to_base_format(const GLenum internalFormat)
{
	// See the explanation on the `format` argument:
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml

	switch(internalFormat)
	{
	case GL_R8: 
	case GL_R16F:
		return GL_RED;

	case GL_RG8:
	case GL_RG16F:
		return GL_RG;

	case GL_RGB8:
	case GL_RGB16F:
	case GL_RGB32F:
		return GL_RGB;

	case GL_RGBA8:
	case GL_RGBA16F:
	case GL_RGBA32F:
		return GL_RGBA;

	case GL_DEPTH24_STENCIL8:
	case GL_DEPTH32F_STENCIL8:
		return GL_DEPTH_STENCIL;

	case GL_DEPTH_COMPONENT32F:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT16:
		return GL_DEPTH_COMPONENT;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

uint32 num_pixel_components(const GLenum internalFormat)
{
	switch(internalFormat)
	{
	case GL_R8: 
	case GL_R16F:
		return 1;

	case GL_RG8:
	case GL_RG16F:
		return 2;

	case GL_RGB8:
	case GL_RGB16F:
	case GL_RGB32F:
		return 3;

	case GL_RGBA8:
	case GL_RGBA16F:
	case GL_RGBA32F:
		return 4;

	case GL_DEPTH24_STENCIL8:
	case GL_DEPTH32F_STENCIL8:
		return 2;

	case GL_DEPTH_COMPONENT32F:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT16:
		return 1;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
}

bool is_color_format(const GLenum internalFormat)
{
	switch(internalFormat)
	{
	case GL_R8:
	case GL_R16F:
	case GL_RG8:
	case GL_RG16F:
	case GL_RGB8:
	case GL_RGB16F:
	case GL_RGB32F:
	case GL_RGBA8:
	case GL_RGBA16F:
	case GL_RGBA32F:
		return true;

	case GL_DEPTH24_STENCIL8:
	case GL_DEPTH32F_STENCIL8:
	case GL_DEPTH_COMPONENT32F:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT16:
		return false;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

GLenum translate(const EGHIInfoPixelComponent componentType)
{
	// Reference: https://www.khronos.org/opengl/wiki/OpenGL_Type

	switch(componentType)
	{
	case EGHIInfoPixelComponent::Empty:
		return GL_NONE;

	case EGHIInfoPixelComponent::Int8:
		return GL_BYTE;

	case EGHIInfoPixelComponent::UInt8:
		return GL_UNSIGNED_BYTE;

	case EGHIInfoPixelComponent::Int16:
		return GL_SHORT;

	case EGHIInfoPixelComponent::UInt16:
		return GL_UNSIGNED_SHORT;

	case EGHIInfoPixelComponent::Int32:
		return GL_INT;

	case EGHIInfoPixelComponent::UInt32:
		return GL_UNSIGNED_INT;

	case EGHIInfoPixelComponent::Float16:
		return GL_HALF_FLOAT;

	case EGHIInfoPixelComponent::Float32:
		return GL_FLOAT;

	case EGHIInfoPixelComponent::Float64:
		return GL_DOUBLE;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum translate(const EGHIInfoStorageElement elementType)
{
	// Reference: https://www.khronos.org/opengl/wiki/OpenGL_Type

	switch(elementType)
	{
	case EGHIInfoStorageElement::Empty:
		return GL_NONE;

	case EGHIInfoStorageElement::Int8:
		return GL_BYTE;

	case EGHIInfoStorageElement::UInt8:
		return GL_UNSIGNED_BYTE;

	case EGHIInfoStorageElement::Int16:
		return GL_SHORT;

	case EGHIInfoStorageElement::UInt16:
		return GL_UNSIGNED_SHORT;

	case EGHIInfoStorageElement::Int32:
		return GL_INT;

	case EGHIInfoStorageElement::UInt32:
		return GL_UNSIGNED_INT;

	case EGHIInfoStorageElement::Float16:
		return GL_HALF_FLOAT;

	case EGHIInfoStorageElement::Float32:
		return GL_FLOAT;

	case EGHIInfoStorageElement::Float64:
		return GL_DOUBLE;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum translate(const EGHIInfoMeshDrawMode drawMode)
{
	switch(drawMode)
	{
	case EGHIInfoMeshDrawMode::Points:
		return GL_POINTS;

	case EGHIInfoMeshDrawMode::LineSegments:
		return GL_LINES;

	case EGHIInfoMeshDrawMode::LineCurveOpened:
		return GL_LINE_STRIP;

	case EGHIInfoMeshDrawMode::LineCurveClosed:
		return GL_LINE_LOOP;

	case EGHIInfoMeshDrawMode::TriangleStrip:
		return GL_TRIANGLE_STRIP;

	case EGHIInfoMeshDrawMode::TriangleFan:
		return GL_TRIANGLE_FAN;

	case EGHIInfoMeshDrawMode::Triangles:
		return GL_TRIANGLES;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

}// end namespace ph::editor::opengl
