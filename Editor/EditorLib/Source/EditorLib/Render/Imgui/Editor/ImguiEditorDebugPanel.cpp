#include "EditorLib/Render/Imgui/Editor/ImguiEditorDebugPanel.h"
#include "EditorLib/App/Editor.h"
#include "EditorLib/Designer/DesignerScene.h"
#include "EditorLib/Designer/TextualNoteObject.h"
#include "EditorLib/Render/Imgui/Font/imgui_icons.h"

#include "EditorLib/ThirdParty/DearImGui.h"

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
