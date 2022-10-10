#pragma once

#include "App/Event/Event.h"
#include "EditorCore/EKeyCode.h"

#include <Utility/TBitFlags.h>

namespace ph::editor
{

class KeyEvent : public Event
{
public:
	explicit KeyEvent(EKeyCode key);

	EEventType getDynamicType() const override = 0;
	std::string toString() const override = 0;

	EEventSource getSourceType() const override;

	EKeyCode getKey() const;

private:
	EKeyCode m_key;
};

inline KeyEvent::KeyEvent(const EKeyCode key)
	: Event()
	, m_key(key)
{}

inline EEventSource KeyEvent::getSourceType() const
{
	return TEnumFlags<EEventSource>(
		{EEventSource::App, EEventSource::Input, EEventSource::Keyboard}).getEnum();
}

inline EKeyCode KeyEvent::getKey() const
{
	return m_key;
}

}// end namespace ph::editor
