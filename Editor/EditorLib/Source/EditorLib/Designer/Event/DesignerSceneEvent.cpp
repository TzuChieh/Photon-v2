#include "EditorLib/Designer/Event/DesignerSceneEvent.h"
#include "EditorLib/Designer/DesignerScene.h"

namespace ph::editor
{

Editor& DesignerSceneEvent::getEditor() const
{
	return getScene().getEditor();
}

bool DesignerSceneEvent::isInitialized(DesignerObject* const obj)
{
	return obj && DesignerScene::isInitialized(*obj);
}

}// end namespace ph::editor
