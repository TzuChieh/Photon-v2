#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"
#include "Designer/UI/UIPropertyLayout.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include <SDL/SdlResourceId.h>

#include <vector>

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
	void buildGeneralSettings(DesignerObject& obj);
	void buildPropertyEditor(DesignerObject& obj);
	void buildPropertiesInGroup(const UIPropertyGroup& group);

	SdlResourceId m_layoutObjID;
	UIPropertyLayout m_propertyLayout;
	imgui::StringCache m_stringEditCache;
	imgui::StringCache m_popupNameEditCache;
};

}// end namespace ph::editor
