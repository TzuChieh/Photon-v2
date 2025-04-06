#pragma once

#include "EditorLib/RenderCore/GraphicsContext.h"
#include "EditorLib/RenderCore/OpenGL/OpenglGHI.h"
#include "EditorLib/RenderCore/OpenGL/OpenglObjectManager.h"
#include "EditorLib/RenderCore/OpenGL/OpenglMemoryManager.h"

#include "EditorLib/ThirdParty/GLFW3_fwd.h"

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
