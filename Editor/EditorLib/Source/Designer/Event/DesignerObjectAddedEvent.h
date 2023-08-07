#pragma once

#include "Designer/Event/DesignerSceneEvent.h"

#include <Common/assertion.h>

namespace ph::editor
{

class DesignerObject;

/*! @brief Event for an object that is added.
*/
class DesignerObjectAddedEvent final : public DesignerSceneEvent
{
public:
	DesignerObjectAddedEvent(DesignerObject* obj, DesignerScene* scene);

	/*!
	@return The added object.
	*/
	DesignerObject& getObject() const;

private:
	DesignerObject* m_obj;
};

inline DesignerObjectAddedEvent::DesignerObjectAddedEvent(
	DesignerObject* const obj, DesignerScene* const scene)

	: DesignerSceneEvent(scene)

	, m_obj(obj)
{}

inline DesignerObject& DesignerObjectAddedEvent::getObject() const
{
	PH_ASSERT(m_obj);
	return *m_obj;
}

}// end namespace ph::editor
