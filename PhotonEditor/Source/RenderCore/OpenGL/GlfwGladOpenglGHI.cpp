#include "RenderCore/OpenGL/GlfwGladOpenglGHI.h"

#include <Common/assertion.h>

#include <GLFW/glfw3.h>

namespace ph::editor
{

GlfwGladOpenglGHI::GlfwGladOpenglGHI(GLFWwindow* const glfwWindow)
	: GHI()
	, m_glfwWindow(glfwWindow)
{
	PH_ASSERT(glfwWindow);

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
