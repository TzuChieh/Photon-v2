#pragma once

#include "EditorLib/Platform/Platform.h"
#include "EditorLib/Platform/GlfwPlatform/GlfwInput.h"
#include "EditorLib/Platform/GlfwPlatform/GlfwDisplay.h"

#include "EditorLib/ThirdParty/GLFW3_fwd.h"

#include <memory>

namespace ph::editor
{

class AppSettings;
class Editor;

class GlfwPlatform : public Platform
{
public:
	GlfwPlatform(const AppSettings& settings, Editor& editor);
	~GlfwPlatform() override;

	void update(float64 deltaS) override;
	const PlatformInput& getInput() const override;
	const PlatformDisplay& getDisplay() const override;

	bool isGlfwWindowInitialized() const;

private:
	void initialize(const AppSettings& settings);
	void terminate();

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
	return m_display.getGlfwWindow() != nullptr;
}

}// end namespace ph::editor
