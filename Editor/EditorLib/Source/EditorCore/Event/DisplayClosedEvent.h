#pragma once

#include "EditorCore/Event/Event.h"

namespace ph::editor
{

class DisplayClosedEvent final : public Event
{
public:
	inline DisplayClosedEvent() = default;
};

}// end namespace ph::editor
