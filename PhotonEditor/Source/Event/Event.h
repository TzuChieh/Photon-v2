#pragma once

#include <Common/primitive_type.h>
#include <Math/math.h>
#include <Common/assertion.h>

#include <string>

namespace ph::editor
{

enum class EEventType
{
	Unknown = 0,

	// TODO: distanguish between main window and window (child/internal)?

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

enum class EEventSource : uint32
{
	Unknown = 0,

	App         = math::flag_bit<uint32, 0>(),
	Input       = math::flag_bit<uint32, 1>(),
	Keyboard    = math::flag_bit<uint32, 2>(),
	Mouse       = math::flag_bit<uint32, 3>(),
	MouseButton = math::flag_bit<uint32, 4>(),
};

class Event
{
public:
	virtual ~Event() = default;

	virtual EEventType getDynamicType() const = 0;
	virtual EEventSource getSourceType() const = 0;
	virtual std::string toString() const = 0;

	void consume();
	bool isConsumed() const;
	bool isFromSource(EEventSource fromSourceType) const;

private:
	bool m_isConsumed = false;
};

inline void Event::consume()
{
	PH_ASSERT_MSG(!m_isConsumed, "Consuming already-consumed editor event.");

	m_isConsumed = true;
}

inline bool Event::isConsumed() const
{
	return m_isConsumed;
}

}// end namespace ph::editor