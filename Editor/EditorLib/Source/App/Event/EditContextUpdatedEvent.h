#pragma once

#include "App/Event/TEditorEvent.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph::editor
{

class EditContext;

enum class EEditContextEvent : uint8
{
	None = 0,
	ActiveSceneChanged
};

class EditContextUpdatedEvent final : public TEditorEvent<true>
{
public:
	EditContextUpdatedEvent(EEditContextEvent type, Editor* editor);

	EditContext getContext() const;
	EEditContextEvent getType() const;

private:
	EEditContextEvent m_type;
};

inline EditContextUpdatedEvent::EditContextUpdatedEvent(
	const EEditContextEvent type,
	Editor* const editor)

	: TEditorEvent(editor)

	, m_type(type)
{}

inline EEditContextEvent EditContextUpdatedEvent::getType() const
{
	return m_type;
}

}// end namespace ph::editor
