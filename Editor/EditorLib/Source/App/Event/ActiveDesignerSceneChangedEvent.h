#pragma once

#include "App/Event/TEditorEvent.h"

namespace ph::editor
{

class DesignerScene;

class ActiveDesignerSceneChangedEvent final : public TEditorEvent<false>
{
public:
	ActiveDesignerSceneChangedEvent(DesignerScene* scene, Editor* editor);

	/*!
	@return The new active scene. May be null.
	*/
	DesignerScene* getScene() const;

private:
	DesignerScene* m_scene;
};

inline ActiveDesignerSceneChangedEvent::ActiveDesignerSceneChangedEvent(
	DesignerScene* const scene, Editor* const editor)

	: TEditorEvent(editor)

	, m_scene(scene)
{}

inline DesignerScene* ActiveDesignerSceneChangedEvent::getScene() const
{
	return m_scene;
}

}// end namespace ph::editor
