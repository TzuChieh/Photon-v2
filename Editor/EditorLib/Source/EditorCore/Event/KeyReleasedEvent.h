#pragma once

#include "EditorCore/Event/KeyEvent.h"

namespace ph::editor
{

class KeyReleasedEvent final : public KeyEvent
{
public:
	explicit KeyReleasedEvent(EKeyCode key);
};

inline KeyReleasedEvent::KeyReleasedEvent(const EKeyCode key)
	: KeyEvent(key)
{}

}// end namespace ph::editor
