#include "RenderCore/OpenGL/OpenglContext.h"

namespace ph::editor::ghi
{

OpenglContext::OpenglContext(GLFWwindow* const glfwWindow, const bool hasDebugContext)

	: GraphicsContext()

	, m_ghi(*this, glfwWindow, hasDebugContext)
	, m_objManager(*this)
	, m_memManager(*this)
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

OpenglMemoryManager& OpenglContext::getMemoryManager()
{
	return m_memManager;
}

}// end namespace ph::editor::ghi
