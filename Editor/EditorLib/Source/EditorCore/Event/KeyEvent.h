#pragma once

#include "EditorCore/Event/Event.h"
#include "EditorCore/EKeyCode.h"

#include <Utility/TBitFlags.h>
#include <Utility/utility.h>

namespace ph::editor
{

class KeyEvent : public Event
{
// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(KeyEvent);

public:
	explicit KeyEvent(EKeyCode key);

	/*EEventType getDynamicType() const override = 0;
	std::string toString() const override = 0;*/

	//EEventSource getSourceType() const override;

	EKeyCode getKey() const;

private:
	EKeyCode m_key;
};

inline KeyEvent::KeyEvent(const EKeyCode key)
	: Event()
	, m_key(key)
{}

//inline EEventSource KeyEvent::getSourceType() const
//{
//	return TEnumFlags<EEventSource>(
//		{EEventSource::App, EEventSource::Input, EEventSource::Keyboard}).getEnum();
//}

inline EKeyCode KeyEvent::getKey() const
{
	return m_key;
}

}// end namespace ph::editor
