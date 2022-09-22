#pragma once

#include "RenderCore/GHI.h"

struct GLFWwindow;

namespace ph::editor
{

class GlfwGladOpenglGHI : public GHI
{
public:
	explicit GlfwGladOpenglGHI(GLFWwindow* glfwWindow);
	~GlfwGladOpenglGHI() override;

	void swapBuffers() override;

private:
	GLFWwindow* m_glfwWindow;
};

}// end namespace ph::editor
