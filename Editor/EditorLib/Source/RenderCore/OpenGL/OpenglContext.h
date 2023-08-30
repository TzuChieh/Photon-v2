#pragma once

#include "RenderCore/GraphicsContext.h"
#include "RenderCore/OpenGL/OpenglGHI.h"
#include "RenderCore/OpenGL/OpenglObjectManager.h"

#include "ThirdParty/GLFW3_fwd.h"

namespace ph::editor
{

class OpenglContext final : public GraphicsContext
{
public:
	OpenglContext(GLFWwindow* glfwWindow, bool hasDebugContext);
	~OpenglContext() override;

	OpenglGHI& getGHI() override;
	OpenglObjectManager& getObjectManager() override;

private:
	OpenglGHI m_ghi;
	OpenglObjectManager m_objManager;
};

}// end namespace ph::editor
