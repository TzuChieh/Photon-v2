#pragma once

#include "EditorCore/Event/KeyEvent.h"

namespace ph::editor
{

class KeyDownEvent final : public KeyEvent
{
public:
	explicit KeyDownEvent(EKeyCode key);
	KeyDownEvent(EKeyCode key, bool isRepeating);

	/*EEventType getDynamicType() const override;
	std::string toString() const override;*/

	bool isRepeating() const;

private:
	bool m_isRepeating;
};

inline KeyDownEvent::KeyDownEvent(const EKeyCode key)
	: KeyDownEvent(key, false)
{}

inline KeyDownEvent::KeyDownEvent(const EKeyCode key, const bool isRepeating)
	: KeyEvent(key)
	, m_isRepeating(isRepeating)
{}

//inline EEventType KeyDownEvent::getDynamicType() const
//{
//	return EEventType::KeyDown;
//}

//inline std::string KeyDownEvent::toString() const
//{
//	return "Key Down Event";
//}

inline bool KeyDownEvent::isRepeating() const
{
	return m_isRepeating;
}

}// end namespace ph::editor
