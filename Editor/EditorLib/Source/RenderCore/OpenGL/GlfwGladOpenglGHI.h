#pragma once

#include "RenderCore/GHI.h"
#include "ThirdParty/GLFW3_fwd.h"

namespace ph::editor
{

class GlfwGladOpenglGHI : public GHI
{
public:
	explicit GlfwGladOpenglGHI(GLFWwindow* glfwWindow);
	~GlfwGladOpenglGHI() override;

	void load() override;
	void unload() override;
	void swapBuffers() override;

private:
	GLFWwindow* m_glfwWindow;
	bool        m_isLoaded;
};

}// end namespace ph::editor
