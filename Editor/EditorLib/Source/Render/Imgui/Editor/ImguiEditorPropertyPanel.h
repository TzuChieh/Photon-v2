#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"
#include "Designer/UI/UIPropertyLayout.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include <Common/primitive_type.h>
#include <SDL/SdlResourceId.h>

#include <vector>
#include <string_view>
#include <unordered_map>

namespace ph { class SdlEnum; }

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
	struct EnumType
	{
		const SdlEnum* enuw = nullptr;
		std::unordered_map<int64, std::string> valueToDisplayName;

		const char* getDisplayName(int64 enumValue) const
		{
			auto iter = valueToDisplayName.find(enumValue);
			return iter != valueToDisplayName.end() ? iter->second.c_str() : "(unknown entry)";
		}
	};

	void buildGeneralSettings(DesignerObject& obj);
	void buildPropertyEditor(DesignerObject& obj);
	void buildPropertiesInGroup(const UIPropertyGroup& group);

	static auto gatherEnumTypes() -> std::vector<EnumType>;
	static auto getEnumType(std::string_view typeSignature) -> const EnumType*;

	SdlResourceId m_layoutObjID;
	UIPropertyLayout m_propertyLayout;
	imgui::StringCache m_stringEditCache;
	imgui::StringCache m_popupNameEditCache;
};

}// end namespace ph::editor
