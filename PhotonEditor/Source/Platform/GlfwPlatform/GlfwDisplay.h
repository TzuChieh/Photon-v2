#pragma once

#include "Platform/Display.h"

#include <string>
#include <memory>

struct GLFWwindow;

namespace ph::editor
{

class GlfwDisplay : public Display
{
public:
	GlfwDisplay();

	GHI* getGHI() const override;
	math::Vector2S getSizePx() const override;

	bool createWindow(
		const std::string& windowTitle,
		math::Vector2S     sizePx);

	void destroyWindow();

	GLFWwindow* getGlfwWindow() const;

private:
	GLFWwindow*          m_glfwWindow; 
	std::unique_ptr<GHI> m_ghi;
	math::Vector2S       m_sizePx;
};

inline GLFWwindow* GlfwDisplay::getGlfwWindow() const
{
	return m_glfwWindow;
}

}// end namespace ph::editor
