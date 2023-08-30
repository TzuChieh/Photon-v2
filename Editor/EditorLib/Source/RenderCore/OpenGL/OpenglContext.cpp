#include "RenderCore/OpenGL/OpenglContext.h"

namespace ph::editor
{

OpenglContext::OpenglContext(GLFWwindow* const glfwWindow, const bool hasDebugContext)

	: GraphicsContext()

	, m_ghi(*this, glfwWindow, hasDebugContext)
	, m_objManager(*this)
{}

OpenglContext::~OpenglContext() = default;

OpenglGHI& OpenglContext::getGHI()
{
	return m_ghi;
}

OpenglObjectManager& OpenglContext::getObjectManager()
{
	return m_objManager;
}

}// end namespace ph::editor
