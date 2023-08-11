#include "Render/Imgui/Editor/ImguiEditorDebugPanel.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/TextualNoteObject.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>

namespace ph::editor
{

ImguiEditorDebugPanel::ImguiEditorDebugPanel()
	: m_objectNameBuffer(128, '\0')
{}

void ImguiEditorDebugPanel::buildWindow(
	const char* title, 
	ImguiEditorUIProxy editorUI,
	bool* const isOpening)
{
	if(!ImGui::Begin(title, isOpening))
	{
		ImGui::End();
		return;
	}

	if(ImGui::Button("Add Note Object"))
	{
		DesignerScene* scene = editorUI.getEditor().getActiveScene();
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

}// end namespace ph::editor
