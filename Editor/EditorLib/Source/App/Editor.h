#pragma once

#include "App/TEventDispatcher.h"
#include "App/Event/KeyDownEvent.h"

namespace ph::editor
{

class Editor final
{
public:

public:
	TEventDispatcher<KeyDownEvent> onKeyDown;

private:
};

}// end namespace ph::editor
