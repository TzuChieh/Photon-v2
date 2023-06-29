#pragma once

#include "EditorCore/Event/KeyEvent.h"

#include <Common/primitive_type.h>

namespace ph::editor
{

class KeyPressedEvent final : public KeyEvent
{
public:
	explicit KeyPressedEvent(EKeyCode key);
	KeyPressedEvent(EKeyCode key, bool isRepeating);

	/*EEventType getDynamicType() const override;
	std::string toString() const override;*/

	bool isRepeating() const;

private:
	uint8 m_isRepeating : 1;
};

inline KeyPressedEvent::KeyPressedEvent(const EKeyCode key)
	: KeyPressedEvent(key, false)
{}

inline KeyPressedEvent::KeyPressedEvent(const EKeyCode key, const bool isRepeating)
	: KeyEvent(key)
	, m_isRepeating(isRepeating)
{}

//inline EEventType KeyPressedEvent::getDynamicType() const
//{
//	return EEventType::KeyDown;
//}

//inline std::string KeyPressedEvent::toString() const
//{
//	return "Key Down Event";
//}

inline bool KeyPressedEvent::isRepeating() const
{
	return m_isRepeating;
}

}// end namespace ph::editor
