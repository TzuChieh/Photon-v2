#pragma once

#include "RenderCore/GraphicsContext.h"
#include "RenderCore/OpenGL/OpenglGHI.h"
#include "RenderCore/OpenGL/OpenglObjectManager.h"
#include "RenderCore/OpenGL/OpenglMemoryManager.h"

#include "ThirdParty/GLFW3_fwd.h"

namespace ph::editor::ghi
{

class OpenglContext final : public GraphicsContext
{
public:
	OpenglContext(GLFWwindow* glfwWindow, bool hasDebugContext);
	~OpenglContext() override;

	OpenglGHI& getGHI() override;
	OpenglObjectManager& getObjectManager() override;
	OpenglMemoryManager& getMemoryManager() override;

private:
	OpenglGHI m_ghi;
	OpenglObjectManager m_objManager;
	OpenglMemoryManager m_memManager;
};

}// end namespace ph::editor::ghi
