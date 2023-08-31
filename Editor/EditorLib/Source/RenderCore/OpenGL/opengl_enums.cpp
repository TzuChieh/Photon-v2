#include "RenderCore/OpenGL/opengl_enums.h"

#include <Common/assertion.h>

namespace ph::editor::opengl
{

GLenum to_internal_format(const EGHIPixelFormat format)
{
	switch(format)
	{
	case EGHIPixelFormat::Empty: return GL_NONE;
	case EGHIPixelFormat::RGB_8: return GL_RGB8;
	case EGHIPixelFormat::RGBA_8: return GL_RGBA8;
	case EGHIPixelFormat::RGB_16F: return GL_RGB16F;
	case EGHIPixelFormat::RGBA_16F: return GL_RGBA16F;
	case EGHIPixelFormat::RGB_32F: return GL_RGB32F;
	case EGHIPixelFormat::RGBA_32F: return GL_RGBA32F;
	case EGHIPixelFormat::Depth_24_Stencil_8: return GL_DEPTH24_STENCIL8;
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

GLenum translate(const EGHIPixelComponent componentType)
{
	// Reference: https://www.khronos.org/opengl/wiki/OpenGL_Type

	switch(componentType)
	{
	case EGHIPixelComponent::Empty:
		return GL_NONE;

	case EGHIPixelComponent::Int8:
		return GL_BYTE;

	case EGHIPixelComponent::UInt8:
		return GL_UNSIGNED_BYTE;

	case EGHIPixelComponent::Int16:
		return GL_SHORT;

	case EGHIPixelComponent::UInt16:
		return GL_UNSIGNED_SHORT;

	case EGHIPixelComponent::Int32:
		return GL_INT;

	case EGHIPixelComponent::UInt32:
		return GL_UNSIGNED_INT;

	case EGHIPixelComponent::Float16:
		return GL_HALF_FLOAT;

	case EGHIPixelComponent::Float32:
		return GL_FLOAT;

	case EGHIPixelComponent::Float64:
		return GL_DOUBLE;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum translate(const EGHIStorageElement elementType)
{
	// Reference: https://www.khronos.org/opengl/wiki/OpenGL_Type

	switch(elementType)
	{
	case EGHIStorageElement::Empty:
		return GL_NONE;

	case EGHIStorageElement::Int8:
		return GL_BYTE;

	case EGHIStorageElement::UInt8:
		return GL_UNSIGNED_BYTE;

	case EGHIStorageElement::Int16:
		return GL_SHORT;

	case EGHIStorageElement::UInt16:
		return GL_UNSIGNED_SHORT;

	case EGHIStorageElement::Int32:
		return GL_INT;

	case EGHIStorageElement::UInt32:
		return GL_UNSIGNED_INT;

	case EGHIStorageElement::Float16:
		return GL_HALF_FLOAT;

	case EGHIStorageElement::Float32:
		return GL_FLOAT;

	case EGHIStorageElement::Float64:
		return GL_DOUBLE;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum translate(const EGHIMeshDrawMode drawMode)
{
	switch(drawMode)
	{
	case EGHIMeshDrawMode::Points:
		return GL_POINTS;

	case EGHIMeshDrawMode::LineSegments:
		return GL_LINES;

	case EGHIMeshDrawMode::LineCurveOpened:
		return GL_LINE_STRIP;

	case EGHIMeshDrawMode::LineCurveClosed:
		return GL_LINE_LOOP;

	case EGHIMeshDrawMode::TriangleStrip:
		return GL_TRIANGLE_STRIP;

	case EGHIMeshDrawMode::TriangleFan:
		return GL_TRIANGLE_FAN;

	case EGHIMeshDrawMode::Triangles:
		return GL_TRIANGLES;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum translate(const EGHIShadingStage shadingStage)
{
	switch(shadingStage)
	{
	case EGHIShadingStage::Vertex:
		return GL_VERTEX_SHADER;

	case EGHIShadingStage::Fragment:
		return GL_FRAGMENT_SHADER;

	case EGHIShadingStage::Compute:
		return GL_COMPUTE_SHADER;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum translate(const EGHIFilterMode filterMode)
{
	switch(filterMode)
	{
	case EGHIFilterMode::Point:
		return GL_NEAREST;

	case EGHIFilterMode::Linear:
		return GL_LINEAR;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum translate(const EGHIWrapMode wrapMode)
{
	switch(wrapMode)
	{
	case EGHIWrapMode::ClampToEdge:
		return GL_CLAMP_TO_EDGE;

	case EGHIWrapMode::Repeat:
		return GL_REPEAT;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

}// end namespace ph::editor::opengl
