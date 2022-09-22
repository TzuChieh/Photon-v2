#include "RenderCore/OpenGL/GlfwGladOpenglGHI.h"
#include "ThirdParty/GLFW3.h"

#include <Common/assertion.h>

namespace ph::editor
{

GlfwGladOpenglGHI::GlfwGladOpenglGHI(GLFWwindow* const glfwWindow)
	: GHI()
	, m_glfwWindow(glfwWindow)
{
	PH_ASSERT(glfwWindow);

	glfwMakeContextCurrent(m_glfwWindow);

	// TODO
}

GlfwGladOpenglGHI::~GlfwGladOpenglGHI()
{
	// TODO
}

void GlfwGladOpenglGHI::swapBuffers()
{
	glfwSwapBuffers(m_glfwWindow);
}

}// end namespace ph::editor
