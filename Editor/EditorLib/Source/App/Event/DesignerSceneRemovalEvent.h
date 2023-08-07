#pragma once

#include "App/Event/TEditorEvent.h"

#include <Common/assertion.h>

namespace ph::editor
{

class DesignerScene;

/*! @brief Event for a scene that is going to be removed.
*/
class DesignerSceneRemovalEvent final : public TEditorEvent<false>
{
public:
	DesignerSceneRemovalEvent(DesignerScene* scene, Editor* editor);

	/*!
	@return The removed scene.
	*/
	DesignerScene& getScene() const;

private:
	DesignerScene* m_scene;
};

inline DesignerSceneRemovalEvent::DesignerSceneRemovalEvent(
	DesignerScene* const scene, Editor* const editor)

	: TEditorEvent(editor)

	, m_scene(scene)
{}

inline DesignerScene& DesignerSceneRemovalEvent::getScene() const
{
	PH_ASSERT(m_scene);
	return *m_scene;
}

}// end namespace ph::editor
