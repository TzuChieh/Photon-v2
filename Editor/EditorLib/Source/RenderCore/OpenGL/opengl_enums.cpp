#include "RenderCore/OpenGL/opengl_enums.h"

#include <Common/assertion.h>

namespace ph::editor::ghi::opengl
{

GLenum to_internal_format(ESizedPixelFormat sizedFormat)
{
	switch(sizedFormat)
	{
	case ESizedPixelFormat::Empty: return GL_NONE;
	case ESizedPixelFormat::RGB_8: return GL_RGB8;
	case ESizedPixelFormat::RGBA_8: return GL_RGBA8;
	case ESizedPixelFormat::RGB_16F: return GL_RGB16F;
	case ESizedPixelFormat::RGBA_16F: return GL_RGBA16F;
	case ESizedPixelFormat::RGB_32F: return GL_RGB32F;
	case ESizedPixelFormat::RGBA_32F: return GL_RGBA32F;
	case ESizedPixelFormat::Depth_24_Stencil_8: return GL_DEPTH24_STENCIL8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return GL_NONE;
	}
}

GLenum to_internal_format(EPixelFormat unsizedFormat)
{
	switch(unsizedFormat)
	{
	case EPixelFormat::Empty: return GL_NONE;
	case EPixelFormat::R: return GL_RED;
	case EPixelFormat::RG: return GL_RG;
	case EPixelFormat::RGB: return GL_RGB;
	case EPixelFormat::RGBA: return GL_RGBA;
	case EPixelFormat::Depth: return GL_DEPTH_COMPONENT;
	case EPixelFormat::Stencil: return GL_STENCIL_INDEX;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return GL_NONE;
	}
}

GLenum to_color_attachment(uint32 slotIndex)
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

GLenum to_data_type(EPixelComponent componentType)
{
	// Reference: https://www.khronos.org/opengl/wiki/OpenGL_Type

	switch(componentType)
	{
	case EPixelComponent::Empty:
		return GL_NONE;

	case EPixelComponent::Int8:
		return GL_BYTE;

	case EPixelComponent::UInt8:
		return GL_UNSIGNED_BYTE;

	case EPixelComponent::Int16:
		return GL_SHORT;

	case EPixelComponent::UInt16:
		return GL_UNSIGNED_SHORT;

	case EPixelComponent::Int32:
		return GL_INT;

	case EPixelComponent::UInt32:
		return GL_UNSIGNED_INT;

	case EPixelComponent::Float16:
		return GL_HALF_FLOAT;

	case EPixelComponent::Float32:
		return GL_FLOAT;

	case EPixelComponent::Float64:
		return GL_DOUBLE;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum to_data_type(EStorageElement elementType)
{
	// Reference: https://www.khronos.org/opengl/wiki/OpenGL_Type

	switch(elementType)
	{
	case EStorageElement::Empty:
		return GL_NONE;

	case EStorageElement::Int8:
		return GL_BYTE;

	case EStorageElement::UInt8:
		return GL_UNSIGNED_BYTE;

	case EStorageElement::Int16:
		return GL_SHORT;

	case EStorageElement::UInt16:
		return GL_UNSIGNED_SHORT;

	case EStorageElement::Int32:
		return GL_INT;

	case EStorageElement::UInt32:
		return GL_UNSIGNED_INT;

	case EStorageElement::Float16:
		return GL_HALF_FLOAT;

	case EStorageElement::Float32:
		return GL_FLOAT;

	case EStorageElement::Float64:
		return GL_DOUBLE;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum to_primitive_type(EMeshDrawMode drawMode)
{
	switch(drawMode)
	{
	case EMeshDrawMode::Points:
		return GL_POINTS;

	case EMeshDrawMode::LineSegments:
		return GL_LINES;

	case EMeshDrawMode::LineCurveOpened:
		return GL_LINE_STRIP;

	case EMeshDrawMode::LineCurveClosed:
		return GL_LINE_LOOP;

	case EMeshDrawMode::TriangleStrip:
		return GL_TRIANGLE_STRIP;

	case EMeshDrawMode::TriangleFan:
		return GL_TRIANGLE_FAN;

	case EMeshDrawMode::Triangles:
		return GL_TRIANGLES;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum to_shader_type(EShadingStage shadingStage)
{
	switch(shadingStage)
	{
	case EShadingStage::Vertex:
		return GL_VERTEX_SHADER;

	case EShadingStage::Fragment:
		return GL_FRAGMENT_SHADER;

	case EShadingStage::Compute:
		return GL_COMPUTE_SHADER;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum to_filter_type(EFilterMode filterMode)
{
	switch(filterMode)
	{
	case EFilterMode::Point:
		return GL_NEAREST;

	case EFilterMode::Linear:
		return GL_LINEAR;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum to_wrap_type(EWrapMode wrapMode)
{
	switch(wrapMode)
	{
	case EWrapMode::ClampToEdge:
		return GL_CLAMP_TO_EDGE;

	case EWrapMode::Repeat:
		return GL_REPEAT;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return GL_NONE;
	}
}

GLenum to_base_format(GLenum internalFormat)
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

uint32 num_pixel_components(GLenum internalFormat)
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

bool is_color_format(GLenum internalFormat)
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

}// end namespace ph::editor::ghi::opengl
