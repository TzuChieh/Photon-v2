#pragma once

#include "App/Event/TEditorEvent.h"

#include <Common/assertion.h>

namespace ph::editor
{

class DesignerScene;

class ActiveDesignerSceneChangedEvent final : public TEditorEvent<false>
{
public:
	ActiveDesignerSceneChangedEvent(
		DesignerScene* activatedScene,
		DesignerScene* deactivatedScene,
		Editor* editor);

	/*!
	@return The active scene. May be null.
	*/
	DesignerScene* getActivatedScene() const;

	/*!
	@return The previously active scene (which has been deactived). May be null.
	*/
	DesignerScene* getDeactivatedScene() const;

private:
	DesignerScene* m_activatedScene;
	DesignerScene* m_deactivatedScene;
};

inline ActiveDesignerSceneChangedEvent::ActiveDesignerSceneChangedEvent(
	DesignerScene* const activatedScene,
	DesignerScene* const deactivatedScene,
	Editor* const editor)

	: TEditorEvent(editor)

	, m_activatedScene(activatedScene)
	, m_deactivatedScene(deactivatedScene)
{
	PH_ASSERT(activatedScene != deactivatedScene);
}

inline DesignerScene* ActiveDesignerSceneChangedEvent::getActivatedScene() const
{
	return m_activatedScene;
}

inline DesignerScene* ActiveDesignerSceneChangedEvent::getDeactivatedScene() const
{
	return m_deactivatedScene;
}

}// end namespace ph::editor
