#pragma once

#include "Platform/Display.h"

#include <string>

struct GLFWwindow;

namespace ph::editor
{

class GlfwDisplay : public Display
{
public:
	GlfwDisplay();

	void present() override;
	math::Vector2S getSizePx() const override;

	bool createWindow(
		const std::string& windowTitle,
		math::Vector2S     sizePx);

	void destroyWindow();
	GLFWwindow* getGlfwWindow() const;

private:
	GLFWwindow*    m_glfwWindow; 
	math::Vector2S m_sizePx;
};

inline GLFWwindow* GlfwDisplay::getGlfwWindow() const
{
	return m_glfwWindow;
}

}// end namespace ph::editor
