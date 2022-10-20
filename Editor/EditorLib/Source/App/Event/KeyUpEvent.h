#pragma once

#include "App/Event/KeyEvent.h"

namespace ph::editor
{

class KeyUpEvent final : public KeyEvent
{
public:
	explicit KeyUpEvent(EKeyCode key);
};

inline KeyUpEvent::KeyUpEvent(const EKeyCode key)
	: KeyEvent(key)
{}

}// end namespace ph::editor
