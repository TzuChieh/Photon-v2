#include "App/Event/EditContextUpdatedEvent.h"
#include "App/Editor.h"

namespace ph::editor
{

EditContext EditContextUpdatedEvent::getContext() const
{
	// We do not cache edit context locally in the event object. As editor event may be buffered,
	// caching can cause stale state being used.
	return getEditor().getEditContext();
}

}// end namespace ph::editor
