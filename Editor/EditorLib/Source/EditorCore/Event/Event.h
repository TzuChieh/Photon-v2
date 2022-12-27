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

	DisplayClose,
	DisplayResize,
	DisplayFocus,
	DisplayLostFocus,
	DisplayMove,

	FramebufferResize,

	AppTick,
	AppUpdate,
	AppRender,

	KeyDown,
	KeyUp,
	KeyHit,

	MouseButtonDown,
	MouseButtonUp,
	MouseClick,
	MouseScroll,
	MouseMove,
};

enum class EEventSource : uint32f
{
	Unknown = 0,

	App         = math::flag_bit<uint32f, 0>(),
	Input       = math::flag_bit<uint32f, 1>(),
	Keyboard    = math::flag_bit<uint32f, 2>(),
	Mouse       = math::flag_bit<uint32f, 3>(),
	MouseButton = math::flag_bit<uint32f, 4>(),
};

/*! @brief Base of all event types.
Derived classes should strive to keep the size of the object small, as most of the time event
objects are passed by value for thread safety and ease of lifetime management. The type should
also remain `trivially-copyable`.
*/
class Event
{
// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	inline Event() = default;
	inline Event(const Event& other) = default;
	inline Event(Event&& other) = default;
	inline Event& operator = (const Event& rhs) = default;
	inline Event& operator = (Event&& rhs) = default;
	inline ~Event() = default;

public:
	/*inline Event() = default;
	inline virtual ~Event() = default;*/

	/*virtual EEventType getDynamicType() const = 0;
	virtual EEventSource getSourceType() const = 0;
	virtual std::string toString() const = 0;*/

	void consume();
	bool isConsumed() const;
	//bool isFromSource(EEventSource fromSourceType) const;

private:
	uint8 m_isConsumed : 1 = false;
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
