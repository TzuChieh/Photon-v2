#pragma once

#include "EditorCore/Event/Event.h"

namespace ph::editor
{

class DisplayCloseEvent final : public Event
{
public:
	inline DisplayCloseEvent() = default;
};

}// end namespace ph::editor
