#include "RenderCore/OpenGL/OpenglGHI.h"
#include "ThirdParty/glad2_with_GLFW3.h"
#include "Platform/Platform.h"
#include "RenderCore/OpenGL/opengl_enums.h"
#include "RenderCore/OpenGL/OpenglTexture2D.h"
#include "RenderCore/OpenGL/OpenglFramebuffer.h"
#include "RenderCore/GHIInfoDeviceCapability.h"
#include "RenderCore/OpenGL/Opengl.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/utility.h>
#include <Utility/TBitFlags.h>

#include <string_view>
#include <cstddef>
#include <format>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(OpenglGHI, GHI);

namespace
{

inline std::string_view GLubyte_to_sv(const GLubyte* const ubytes)
{
	PH_ASSERT(ubytes);
	return std::string_view(reinterpret_cast<const char*>(ubytes));
}

inline std::string_view debug_source_GLenum_to_sv(const GLenum dbgSource)
{
	switch(dbgSource)
	{
	case GL_DEBUG_SOURCE_API:
		return "API";

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window System";

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader Compiler";

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third Party";

	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";

	case GL_DEBUG_SOURCE_OTHER:
		return "Other";

	default:
		return "(unknown)";
	}
}

inline std::string_view debug_type_GLenum_to_sv(const GLenum dbgType)
{
	switch(dbgType)
	{
	case GL_DEBUG_TYPE_ERROR:
		return "Error";

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated Behavior";

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined Behavior";

	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability";

	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance";

	case GL_DEBUG_TYPE_OTHER:
		return "Other";

	case GL_DEBUG_TYPE_MARKER:
		return "Marker";

	default:
		return "(unknown)";
	}
}

}// end anonymous namespace

}// end namespace ph::editor

extern "C"
{

inline void APIENTRY ph_editor_OpenGL_debug_callback(
	const GLenum        source,
	const GLenum        type,
	const GLuint        id,
	const GLenum        severity,
	const GLsizei       length,
    const GLchar* const message,
    const void* const   userParam)
{
	using namespace ph::editor;

	auto debugStr = std::format("{} (source: {}, type: {}, id: {})",
		std::string_view(reinterpret_cast<const char*>(message), length),
		debug_source_GLenum_to_sv(source),
		debug_type_GLenum_to_sv(type),
		id);

	switch(severity)
	{
	// Anything that isn't an error or performance issue
	case GL_DEBUG_SEVERITY_NOTIFICATION:
	// Redundant state change performance warning, or unimportant undefined behavior
	case GL_DEBUG_SEVERITY_LOW:
		PH_LOG(OpenglGHI, "{}", debugStr);
		break;

	// Major performance warnings, shader compilation/linking warnings, or the use of deprecated functionality
	case GL_DEBUG_SEVERITY_MEDIUM:
		PH_LOG_WARNING(OpenglGHI, "{}", debugStr);
		break;

	// All OpenGL Errors, shader compilation/linking errors, or highly-dangerous undefined behavior
	case GL_DEBUG_SEVERITY_HIGH:
	default:
		PH_LOG_ERROR(OpenglGHI, "{}", debugStr);
		break;
	}
}

}// end extern "C"

namespace ph::editor
{

OpenglGHI::OpenglGHI(GLFWwindow* const glfwWindow, const bool hasDebugContext)
	: GHI(EGraphicsAPI::OpenGL)
	, m_glfwWindow     (glfwWindow)
	, m_hasDebugContext(hasDebugContext)
	, m_isLoaded       (false)
#ifdef PH_DEBUG
	, m_loadThreadId   ()
#endif
	, m_deviceCapability(nullptr)
{}

OpenglGHI::~OpenglGHI()
{
	PH_ASSERT(!m_isLoaded);
}

void OpenglGHI::load()
{
#ifdef PH_DEBUG
	m_loadThreadId = std::this_thread::get_id();
#endif

	if(m_isLoaded)
	{
		return;
	}

	if(!m_glfwWindow)
	{
		throw PlatformException(
			"invalid GLFW window (null)");
	}

	glfwMakeContextCurrent(m_glfwWindow);

	const int version = gladLoadGL(glfwGetProcAddress);
	if(version == 0)
	{
		throw PlatformException(
			"failed to load OpenGL (glad load failed)");
	}

	if(m_hasDebugContext)
	{
		glDebugMessageCallback(ph_editor_OpenGL_debug_callback, nullptr);
	}

	PH_LOG(OpenglGHI,
		"loaded OpenGL {}.{}", 
		GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	// Log information provided by OpenGL itself
	PH_LOG(OpenglGHI,
		"Vendor: {}, Renderer: {}, Version: {}, GLSL Version: {}",
		GLubyte_to_sv(glGetString(GL_VENDOR)),
		GLubyte_to_sv(glGetString(GL_RENDERER)),
		GLubyte_to_sv(glGetString(GL_VERSION)),
		GLubyte_to_sv(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	m_isLoaded = true;
}

void OpenglGHI::unload()
{
	PH_ASSERT(std::this_thread::get_id() == m_loadThreadId);

	if(m_isLoaded)
	{
		glfwMakeContextCurrent(nullptr);
	}

	m_isLoaded = false;
}

void OpenglGHI::setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx)
{
	glViewport(
		lossless_cast<GLint>(xPx),
		lossless_cast<GLint>(yPx),
		lossless_cast<GLsizei>(widthPx),
		lossless_cast<GLsizei>(heightPx));
}

void OpenglGHI::clearBuffer(const EClearTarget targets)
{
	const TEnumFlags<EClearTarget> flags({targets});

	GLbitfield mask = 0;

	if(flags.hasAny({EClearTarget::Color}))
	{
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if(flags.hasAny({EClearTarget::Depth}))
	{
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if(flags.hasAny({EClearTarget::Stencil}))
	{
		mask |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(mask);
}

void OpenglGHI::setClearColor(const math::Vector4F& color)
{
	glClearColor(
		lossless_cast<GLclampf>(color.r()),
		lossless_cast<GLclampf>(color.g()),
		lossless_cast<GLclampf>(color.b()),
		lossless_cast<GLclampf>(color.a()));
}

void OpenglGHI::swapBuffers()
{
	glfwSwapBuffers(m_glfwWindow);
}

std::shared_ptr<GHITexture2D> OpenglGHI::createTexture2D(
	const GHIInfoTextureFormat& format,
	const math::Vector2UI& sizePx)
{
	return std::make_shared<OpenglTexture2D>(
		format, sizePx);
}

std::shared_ptr<GHIFramebuffer> OpenglGHI::createFramebuffer(
	const GHIInfoFramebufferAttachment& attachments)
{
	return std::make_shared<OpenglFramebuffer>(attachments);
}

GHIInfoDeviceCapability OpenglGHI::getDeviceCapabilities()
{
	if(m_deviceCapability != nullptr)
	{
		return *m_deviceCapability;
	}

	// Reference: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml

	m_deviceCapability = std::make_unique<GHIInfoDeviceCapability>();

	GHIInfoDeviceCapability& c = *m_deviceCapability;
	c.maxTextureUnitsForVertexShadingStage = Opengl::getInteger<uint8>(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	c.maxTextureUnitsForFragmentShadingStage = Opengl::getInteger<uint8>(GL_MAX_TEXTURE_IMAGE_UNITS);
	
	// TODO: also see GL_MAX_VERTEX_ATTRIB_BINDINGS
	c.maxVertexAttributes = Opengl::getInteger<uint8>(GL_MAX_VERTEX_ATTRIBS);
	
	return c;
}

void OpenglGHI::beginRawCommand()
{
	// TODO: currently no state yet
}

void OpenglGHI::endRawCommand()
{
	// TODO: currently no state yet
}

}// end namespace ph::editor
