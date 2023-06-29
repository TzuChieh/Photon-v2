#include "App/Event/EditContextUpdatedEvent.h"
#include "App/Editor.h"

namespace ph::editor
{

EditContext EditContextUpdatedEvent::getContext() const
{
	return getEditor().getEditContext();
}

}// end namespace ph::editor
