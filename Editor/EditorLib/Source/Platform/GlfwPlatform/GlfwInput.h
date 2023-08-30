#pragma once

#include "Platform/PlatformInput.h"

#include "ThirdParty/GLFW3.h"

#include <array>

namespace ph::editor
{

class Editor;

class GlfwInput : public PlatformInput
{
public:
	static const std::array<EKeyCode, GLFW_KEY_LAST + 1>& GLFW_TO_PHOTON_KEYCODE();
	static const std::array<EMouseCode, GLFW_MOUSE_BUTTON_LAST + 1>& GLFW_TO_PHOTON_MOUSECODE();

public:
	GlfwInput();
	~GlfwInput() override;

	void poll(float64 deltaS) override;

	void virtualizeCursor() const override;
	void unvirtualizeCursor() const override;

	bool isKeyDown(EKeyCode keyCode) const override;
	bool isMouseButtonDown(EMouseCode mouseCode) const override;

	bool isKeyUp(EKeyCode keyCode) const override;
	bool isMouseButtonUp(EMouseCode mouseCode) const override;

	bool isKeyHold(EKeyCode keyCode) const override;
	bool isMouseButtonHold(EMouseCode mouseCode) const override;

	math::Vector2D getCursorPositionPx() const override;
	math::Vector2D getCursorMovementDeltaPx() const override;

	void initialize(Editor& editor, GLFWwindow* glfwWindow);
	void terminate();

private:
	GLFWwindow* m_glfwWindow;
};

}// end namespace ph::editor
