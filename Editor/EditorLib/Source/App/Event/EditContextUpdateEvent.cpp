#include "App/Event/EditContextUpdateEvent.h"
#include "App/Editor.h"

namespace ph::editor
{

EditContext EditContextUpdateEvent::getContext() const
{
	return getEditor().getEditContext();
}

}// end namespace ph::editor
