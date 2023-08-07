#pragma once

#include "App/Event/TEditorEvent.h"

#include <Common/assertion.h>

namespace ph::editor
{

class DesignerScene;

/*! @brief Event for a scene that is added.
*/
class DesignerSceneAddedEvent final : public TEditorEvent<false>
{
public:
	DesignerSceneAddedEvent(DesignerScene* scene, Editor* editor);

	/*!
	@return The added scene.
	*/
	DesignerScene& getScene() const;

private:
	DesignerScene* m_scene;
};

inline DesignerSceneAddedEvent::DesignerSceneAddedEvent(
	DesignerScene* const scene, Editor* const editor)

	: TEditorEvent(editor)

	, m_scene(scene)
{}

inline DesignerScene& DesignerSceneAddedEvent::getScene() const
{
	PH_ASSERT(m_scene);
	return *m_scene;
}

}// end namespace ph::editor
