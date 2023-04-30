#pragma once

#include "EditorCore/Event/Event.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph::editor
{

class Editor;
class EditContext;

enum class EEditContextEvent : uint8
{
	None = 0,
	ActiveSceneChanged
};

class EditContextUpdateEvent final : public Event
{
public:
	EditContextUpdateEvent(Editor* editor, EEditContextEvent type);

	EditContext getContext() const;
	Editor& getEditor() const;
	EEditContextEvent getType() const;

private:
	Editor* m_editor;
	EEditContextEvent m_type;
};

inline EditContextUpdateEvent::EditContextUpdateEvent(
	Editor* const editor,
	const EEditContextEvent type)

	: Event()

	, m_editor(editor)
	, m_type(type)
{
	PH_ASSERT(editor);
}

inline Editor& EditContextUpdateEvent::getEditor() const
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

inline EEditContextEvent EditContextUpdateEvent::getType() const
{
	return m_type;
}

}// end namespace ph::editor
