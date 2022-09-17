#pragma once

#include "Platform/Input.h"

namespace ph::editor
{

class GlfwInput : public Input
{
public:
	void poll(float64 deltaS) override;

	virtual void virtualizeCursor() const = 0;
	virtual void unvirtualizeCursor() const = 0;

	virtual bool isKeyDown(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonDown(EMouseCode mouseCode) const = 0;

	virtual bool isKeyUp(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonUp(EMouseCode mouseCode) const = 0;

	virtual bool isKeyHold(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonHold(EMouseCode mouseCode) const = 0;

	virtual math::Vector2D getCursorPositionPx() const = 0;
	virtual math::Vector2D getCursorMovementDeltaPx() const = 0;
};

}// end namespace ph::editor
