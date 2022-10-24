#include "RenderCore/OpenGL/GlfwGladOpenglGHI.h"
#include "ThirdParty/glad2_with_GLFW3.h"
#include "Platform/Platform.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>
#include <Common/logging.h>

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

}// end anonymous namespace

GlfwGladOpenglGHI::GlfwGladOpenglGHI(GLFWwindow* const glfwWindow)
	: GHI()
	, m_glfwWindow(glfwWindow)
	, m_isLoaded(false)
{}

GlfwGladOpenglGHI::~GlfwGladOpenglGHI()
{
	PH_ASSERT(!m_isLoaded);
}

void GlfwGladOpenglGHI::load()
{
	PH_ASSERT(Threads::isOnGHIThread());

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

	m_isLoaded = true;
}

void GlfwGladOpenglGHI::unload()
{
	PH_ASSERT(Threads::isOnGHIThread());

	m_isLoaded = false;
}

void GlfwGladOpenglGHI::swapBuffers()
{
	glfwSwapBuffers(m_glfwWindow);
}

std::unique_ptr<GHI> GlfwGladOpenglGHI::clone()
{
	return std::make_unique<GlfwGladOpenglGHI>(m_glfwWindow);
}

}// end namespace ph::editor
