#pragma once

namespace ph::editor
{

enum class EEventType
{
	Unknown = 0,

	WindowClose,
	WindowResize,
	WindowFocus,
	WindowLostFocus,
	WindowMove,

	AppTick,
	AppUpdate,
	AppRender,

	KeyPress,
	KeyRelease,
	KeyHit,

	MousePress,
	MouseRelease,
	MouseClick,
	MouseScroll,
	MouseMove,
};

class Event
{
public:

private:

};

}// end namespace ph::editor
