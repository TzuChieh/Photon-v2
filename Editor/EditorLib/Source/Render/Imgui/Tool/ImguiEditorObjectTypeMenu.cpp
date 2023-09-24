#include "Render/Imgui/Tool/ImguiEditorObjectTypeMenu.h"
#include "Render/Imgui/ImguiEditorUIProxy.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "ph_editor.h"
#include "Designer/DesignerScene.h"

#include "ThirdParty/DearImGui.h"

#include <string_view>
#include <algorithm>

namespace ph::editor
{

ImguiEditorObjectTypeMenu::ImguiEditorObjectTypeMenu()
{}

void ImguiEditorObjectTypeMenu::buildMenuButton(
	const char* name,
	const SdlClass*& out_selectedClass)
{
	out_selectedClass = nullptr;

	if(ImGui::BeginPopup(name))
	{
		ImGui::MenuItem(PH_IMGUI_OBJECTS_ICON " Object Types", nullptr, false, false);
		ImGui::Separator();
		for(const ObjectType& type : getObjectTypes())
		{
			if(ImGui::MenuItem(type.displayName.c_str()))
			{
				out_selectedClass = type.clazz;
			}
		}
		ImGui::EndPopup();
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.04f, 0.4f, 0.04f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.06f, 0.6f, 0.06f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.055f, 0.55f, 0.055f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

	// Disambiguate popup and button IDs. Also popups require to be on the same ID stack level.
	ImGui::PushID(0);
	const bool isButtonClicked = ImGui::Button(name);
	ImGui::PopID();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);

	if(isButtonClicked)
	{
		ImGui::OpenPopup(name);
	}
}

auto ImguiEditorObjectTypeMenu::gatherObjectTypes()
-> std::vector<ObjectType>
{
	std::vector<ObjectType> types;
	for(const SdlClass* clazz : DesignerScene::getAllObjectClasses())
	{
		if(clazz->isBlueprint())
		{
			continue;
		}

		std::string_view docName = clazz->getDocName();

		// Remove any trailing "Object" or "Designer" as they are redundant in the editor context
		if(docName.ends_with("Object"))
		{
			docName.remove_suffix(6);
			docName = string_utils::trim_tail(docName);
		}
		if(docName.ends_with("Designer"))
		{
			docName.remove_suffix(8);
			docName = string_utils::trim_tail(docName);
		}

		types.push_back({
			.clazz = clazz,
			.displayName = std::string(docName)});
	}

	// Make sure we have same order every time (according to display name)
	std::sort(types.begin(), types.end(),
		[](const ObjectType& a, const ObjectType& b)
		{
			return a.displayName < b.displayName;
		});

	return types;
}

auto ImguiEditorObjectTypeMenu::getObjectTypes()
-> TSpanView<ObjectType>
{
	static std::vector<ObjectType> types = gatherObjectTypes();
	return types;
}

}// end namespace ph::editor
