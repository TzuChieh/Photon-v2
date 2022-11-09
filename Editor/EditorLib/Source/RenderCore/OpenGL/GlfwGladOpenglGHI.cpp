#include "RenderCore/OpenGL/GlfwGladOpenglGHI.h"
#include "ThirdParty/glad2_with_GLFW3.h"
#include "Platform/Platform.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/utility.h>
#include <Utility/TBitFlags.h>

#include <string>
#include <cstddef>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwGladOpenGL, GHI);

namespace
{

inline std::string GLubyte_to_string(const GLubyte* const ubytes)
{
	PH_ASSERT(ubytes);

	std::string str;
	str.reserve(64);
	for(std::size_t i = 0; ubytes[i] != '\0'; ++i)
	{
		str.push_back(static_cast<char>(ubytes[i]));
	}
	return str;
}

inline std::string

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
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
	case GL_DEBUG_SEVERITY_LOW:
	case GL_DEBUG_SEVERITY_MEDIUM:
	case GL_DEBUG_SEVERITY_HIGH:
	}
}

}

namespace ph::editor
{

GlfwGladOpenglGHI::GlfwGladOpenglGHI(GLFWwindow* const glfwWindow, const bool hasDebugContext)
	: GHI(EGraphicsAPI::OpenGL)
	, m_glfwWindow     (glfwWindow)
	, m_hasDebugContext(hasDebugContext)
	, m_isLoaded       (false)
#ifdef PH_DEBUG
	, m_loadThreadId   ()
#endif
{}

GlfwGladOpenglGHI::~GlfwGladOpenglGHI()
{
	PH_ASSERT(!m_isLoaded);
}

void GlfwGladOpenglGHI::load()
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
			"failed to initialize OpenGL context (glad load failed)");
	}

	PH_LOG(GlfwGladOpenGL,
		"loaded OpenGL {}.{}", 
		GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	// Log information provided by OpenGL itself
	PH_LOG(GlfwGladOpenGL,
		"Vendor: {}, Renderer: {}, Version: {}, GLSL Version: {}",
		GLubyte_to_string(glGetString(GL_VENDOR)), 
		GLubyte_to_string(glGetString(GL_RENDERER)), 
		GLubyte_to_string(glGetString(GL_VERSION)), 
		GLubyte_to_string(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	if(m_hasDebugContext)
	{
		glDebugMessageCallback();
	}

	m_isLoaded = true;
}

void GlfwGladOpenglGHI::unload()
{
	PH_ASSERT(std::this_thread::get_id() == m_loadThreadId);

	if(m_isLoaded)
	{
		glfwMakeContextCurrent(nullptr);
	}

	m_isLoaded = false;
}

void GlfwGladOpenglGHI::setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx)
{
	glViewport(
		safe_number_cast<GLint>(xPx),
		safe_number_cast<GLint>(yPx), 
		safe_number_cast<GLsizei>(widthPx),
		safe_number_cast<GLsizei>(heightPx));
}

void GlfwGladOpenglGHI::clearBuffer(const EClearTarget targets)
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

void GlfwGladOpenglGHI::setClearColor(const math::TVector4<float32>& color)
{
	glClearColor(
		safe_number_cast<GLclampf>(color.r()), 
		safe_number_cast<GLclampf>(color.g()), 
		safe_number_cast<GLclampf>(color.b()), 
		safe_number_cast<GLclampf>(color.a()));
}

void GlfwGladOpenglGHI::swapBuffers()
{
	glfwSwapBuffers(m_glfwWindow);
}

void GlfwGladOpenglGHI::beginRawCommand()
{
	// TODO: currently no state yet
}

void GlfwGladOpenglGHI::endRawCommand()
{
	// TODO: currently no state yet
}

}// end namespace ph::editor
