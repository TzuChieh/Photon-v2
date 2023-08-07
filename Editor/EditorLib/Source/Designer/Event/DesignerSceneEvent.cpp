#include "Designer/Event/DesignerSceneEvent.h"
#include "Designer/DesignerScene.h"

namespace ph::editor
{

Editor& DesignerSceneEvent::getEditor() const
{
	return getScene().getEditor();
}

}// end namespace ph::editor
