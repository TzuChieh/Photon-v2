#pragma once

#include "Platform/PlatformDisplay.h"
#include "RenderCore/EGraphicsAPI.h"

#include "ThirdParty/GLFW3_fwd.h"

#include <string>
#include <memory>

namespace ph::editor
{

class Editor;

class GlfwDisplay : public PlatformDisplay
{
public:
	GlfwDisplay();
	~GlfwDisplay() override;

	ghi::GraphicsContext* getGraphicsContext() const override;
	math::Vector2S getSizePx() const override;
	math::Vector2S getFramebufferSizePx() const override;
	NativeWindow getNativeWindow() const override;
	ghi::EGraphicsAPI getGraphicsApiType() const override;
	float32 getDpiScale() const override;

	void initialize(
		Editor&            editor,
		const std::string& windowTitle,
		math::Vector2S     sizePx,
		ghi::EGraphicsAPI  graphicsApi,
		bool               useDebugModeGHI);

	void terminate();

	GLFWwindow* getGlfwWindow() const;

private:
	GLFWwindow*                           m_glfwWindow; 
	std::unique_ptr<ghi::GraphicsContext> m_graphicsCtx;
	ghi::EGraphicsAPI                     m_apiType;
	math::Vector2S                        m_sizePx;
};

inline math::Vector2S GlfwDisplay::getSizePx() const
{
	return m_sizePx;
}

inline GLFWwindow* GlfwDisplay::getGlfwWindow() const
{
	return m_glfwWindow;
}

}// end namespace ph::editor
