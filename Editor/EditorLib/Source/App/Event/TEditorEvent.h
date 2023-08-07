#pragma once

#include "EditorCore/Event/Event.h"

#include <Common/assertion.h>
#include <Utility/utility.h>

namespace ph::editor
{

class Editor;

template<bool CAN_POST>
class TEditorEvent : public Event
{
// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TEditorEvent);

public:
	inline static constexpr bool canPost = CAN_POST;

	explicit TEditorEvent(Editor* editor);

	Editor& getEditor() const;

private:
	Editor* m_editor;
};

template<bool CAN_POST>
inline TEditorEvent<CAN_POST>::TEditorEvent(Editor* const editor)
	: Event()
	, m_editor(editor)
{}

template<bool CAN_POST>
inline Editor& TEditorEvent<CAN_POST>::getEditor() const
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

}// end namespace ph::editor
