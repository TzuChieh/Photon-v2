#pragma once

#include "EditorCore/Event/Event.h"

namespace ph::editor
{

class DisplayFocusChangedEvent final : public Event
{
public:
	explicit DisplayFocusChangedEvent(bool isFocused);

	bool isFocused() const;

private:
	bool m_isFocused;
};

inline DisplayFocusChangedEvent::DisplayFocusChangedEvent(const bool isFocused)
	: Event()
	, m_isFocused(isFocused)
{}

inline bool DisplayFocusChangedEvent::isFocused() const
{
	return m_isFocused;
}

}// end namespace ph::editor
