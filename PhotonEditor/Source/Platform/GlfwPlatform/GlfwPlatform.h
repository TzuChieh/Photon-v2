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
	const Input& getInput() const override;
	const Display& getDisplay() const override;
	bool isInitialized() const override;

	bool isGlfwInitialized() const;

private:
	bool init(const AppSettings& settings);
	void terminate();

	GLFWwindow* m_glfwWindow;
	GlfwInput   m_input;
	GlfwDisplay m_display;
};

inline bool GlfwPlatform::isGlfwInitialized() const
{
	return m_glfwWindow != nullptr;
}

}// end namespace ph::editor
