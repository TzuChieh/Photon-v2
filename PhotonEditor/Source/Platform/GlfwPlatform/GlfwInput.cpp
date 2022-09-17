#include "Platform/GlfwPlatform/GlfwInput.h"

#include <Common/assertion.h>

namespace ph::editor
{

GlfwInput::GlfwInput()
	: Input()
	, m_glfwWindow(nullptr)
{}

GlfwInput::GlfwInput(GLFWwindow* const glfwWindow)
	: Input()
	, m_glfwWindow(glfwWindow)
{
	PH_ASSERT(glfwWindow);
}

}// end namespace ph::editor
