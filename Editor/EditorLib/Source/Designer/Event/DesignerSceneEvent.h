#pragma once

#include "EditorCore/Event/Event.h"

#include <Common/assertion.h>
#include <Utility/utility.h>

namespace ph::editor
{

class DesignerScene;
class DesignerObject;
class Editor;

class DesignerSceneEvent : public Event
{
// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(DesignerSceneEvent);

public:
	explicit DesignerSceneEvent(DesignerScene* scene);

	DesignerScene& getScene() const;
	Editor& getEditor() const;

protected:
	/*! Some event types guarantee an initialized `obj`, this is a convenient helper to check that.
	*/
	static bool isInitialized(DesignerObject* obj);

private:
	DesignerScene* m_scene;
};

inline DesignerSceneEvent::DesignerSceneEvent(DesignerScene* const scene)
	: Event()
	, m_scene(scene)
{}

inline DesignerScene& DesignerSceneEvent::getScene() const
{
	PH_ASSERT(m_scene);
	return *m_scene;
}

}// end namespace ph::editor
