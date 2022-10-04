#pragma once

#include "Platform/PlatformDisplay.h"
#include "App/EGraphicsAPI.h"
#include "ThirdParty/GLFW3_fwd.h"

#include <string>
#include <memory>

namespace ph::editor
{

class GlfwDisplay : public PlatformDisplay
{
public:
	GlfwDisplay();
	~GlfwDisplay() override;

	GHI* getGHI() const override;
	math::Vector2S getSizePx() const override;

	void createWindow(
		const std::string& windowTitle,
		math::Vector2S     sizePx,
		EGraphicsAPI       graphicsApi);

	void closeWindow();

	GLFWwindow* getGlfwWindow() const;

private:
	GLFWwindow*          m_glfwWindow; 
	std::unique_ptr<GHI> m_ghi;
	math::Vector2S       m_sizePx;
};

inline GHI* GlfwDisplay::getGHI() const
{
	return m_ghi.get();
}

inline math::Vector2S GlfwDisplay::getSizePx() const
{
	return m_sizePx;
}

inline GLFWwindow* GlfwDisplay::getGlfwWindow() const
{
	return m_glfwWindow;
}

}// end namespace ph::editor
