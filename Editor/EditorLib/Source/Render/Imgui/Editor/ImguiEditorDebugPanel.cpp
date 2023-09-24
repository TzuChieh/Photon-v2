#include "Render/Imgui/Editor/ImguiEditorDebugPanel.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/TextualNoteObject.h"
#include "Render/Imgui/Font/imgui_icons.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>

namespace ph::editor
{

ImguiEditorDebugPanel::ImguiEditorDebugPanel(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_objectNameBuffer(128, '\0')
{}

void ImguiEditorDebugPanel::buildWindow(const char* windowIdName, bool* isOpening)
{
	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

	if(ImGui::Button("Add Note Object"))
	{
		DesignerScene* scene = getEditorUI().getEditor().getActiveScene();
		if(scene)
		{
			DesignerObject* obj = scene->newRootObject<TextualNoteObject>();
			std::string objectName = m_objectNameBuffer.data();
			obj->setName(objectName);
		}
	}
	ImGui::SameLine();
	ImGui::InputText("Object Name", m_objectNameBuffer.data(), m_objectNameBuffer.size());

	if(ImGui::Button("Assert False"))
	{
		PH_ASSERT(false);
	}

	ImGui::End();
}

auto ImguiEditorDebugPanel::getAttributes() const
-> Attributes
{
	return {
		.title = "Debug Mode",
		.icon = PH_IMGUI_BUG_ICON,
		.useMenubar = false,
		.useSidebar = false};
}

}// end namespace ph::editor
