#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"
#include "Designer/UI/UIPropertyLayout.h"

#include <SDL/SdlResourceId.h>

namespace ph::editor
{

class DesignerScene;
class DesignerObject;

class ImguiEditorPropertyPanel : public ImguiEditorPanel
{
public:
	explicit ImguiEditorPropertyPanel(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	void buildPropertyEditor(DesignerObject& obj);
	void buildPropertiesInGroup(const UIPropertyGroup& group);

	SdlResourceId m_layoutObjID;
	UIPropertyLayout m_propertyLayout;
};

}// end namespace ph::editor
