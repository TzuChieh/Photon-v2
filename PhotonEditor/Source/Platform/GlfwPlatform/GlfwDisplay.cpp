#include "Platform/GlfwPlatform/GlfwDisplay.h"
#include "RenderCore/OpenGL/GlfwGladOpenglGHI.h"

#include <Common/logging.h>

#include <GLFW/glfw3.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwDisplay, EditorPlatform);

GlfwDisplay::GlfwDisplay()
	: Display()
	, m_glfwWindow(nullptr)
	, m_ghi(nullptr)
	, m_sizePx(0, 0)
{}

GHI* GlfwDisplay::getGHI() const
{
	return m_ghi.get();
}

}// end namespace ph::editor
