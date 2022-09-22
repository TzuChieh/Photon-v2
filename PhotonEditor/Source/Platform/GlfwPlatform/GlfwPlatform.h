#pragma once

#include "Platform/Platform.h"
#include "Platform/GlfwPlatform/GlfwInput.h"
#include "Platform/GlfwPlatform/GlfwDisplay.h"

#include <memory>

struct GLFWwindow;

namespace ph::editor
{

class AppSettings;

class GlfwPlatform : public Platform
{
public:
	explicit GlfwPlatform(const AppSettings& settings);
	~GlfwPlatform() override;

	void update(float64 deltaS) override;
	void render() override;
	const PlatformInput& getInput() const override;
	const PlatformDisplay& getDisplay() const override;
	bool isInitialized() const override;

	bool isGlfwWindowInitialized() const;

private:
	void init(const AppSettings& settings);
	void terminate();

	GLFWwindow* m_glfwWindow;
	GlfwInput   m_input;
	GlfwDisplay m_display;
};

inline const PlatformInput& GlfwPlatform::getInput() const
{
	return m_input;
}

inline const PlatformDisplay& GlfwPlatform::getDisplay() const
{
	return m_display;
}

inline bool GlfwPlatform::isGlfwWindowInitialized() const
{
	return m_glfwWindow != nullptr;
}

}// end namespace ph::editor
