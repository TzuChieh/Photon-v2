#pragma once

#include "Designer/Event/DesignerSceneEvent.h"

#include <Common/assertion.h>

namespace ph::editor
{

class DesignerObject;

/*! @brief Event for an object that is going to be removed.
The object has not been removed yet (and still being an initialized object) when this event fires.
*/
class DesignerObjectRemovalEvent final : public DesignerSceneEvent
{
public:
	DesignerObjectRemovalEvent(DesignerObject* obj, DesignerScene* scene);

	/*!
	@return The removed object.
	*/
	DesignerObject& getObject() const;

private:
	DesignerObject* m_obj;
};

inline DesignerObjectRemovalEvent::DesignerObjectRemovalEvent(
	DesignerObject* const obj, DesignerScene* const scene)

	: DesignerSceneEvent(scene)

	, m_obj(obj)
{
	// This is guaranteed by this event
	PH_ASSERT(isInitialized(obj));
}

inline DesignerObject& DesignerObjectRemovalEvent::getObject() const
{
	PH_ASSERT(m_obj);
	return *m_obj;
}

}// end namespace ph::editor
