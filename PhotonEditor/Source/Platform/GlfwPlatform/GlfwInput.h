#pragma once

#include "Platform/Input.h"

struct GLFWwindow;

namespace ph::editor
{

class GlfwInput : public Input
{
public:
	GlfwInput();
	explicit GlfwInput(GLFWwindow* glfwWindow);

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

private:
	GLFWwindow* m_glfwWindow;
};

}// end namespace ph::editor