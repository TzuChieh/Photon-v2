#include "Platform/GlfwPlatform/GlfwInput.h"
#include "Platform/Platform.h"
#include "ThirdParty/GLFW3.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwInput, EditorPlatform);

GlfwInput::GlfwInput()
	: PlatformInput()
	, m_glfwWindow(nullptr)
{}

GlfwInput::~GlfwInput() = default;

void GlfwInput::poll(const float64 deltaS)
{
	PH_ASSERT(m_glfwWindow);

	glfwPollEvents();
}

void GlfwInput::start(GLFWwindow* const glfwWindow)
{
	if(!glfwWindow)
	{
		throw PlatformException(
			"cannot start input due to invalid GLFW window (null)");
	}

	m_glfwWindow = glfwWindow;

	// TODO: callbacks
}

void GlfwInput::stop()
{
	// TODO: cleanup callbacks

	m_glfwWindow = nullptr;
}

}// end namespace ph::editor
