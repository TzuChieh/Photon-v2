#include "Render/Imgui/Editor/ImguiEditorSceneCreator.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"
#include "App/Editor.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

ImguiEditorSceneCreator::ImguiEditorSceneCreator()
	: m_sceneNameBuffer(128, '\0')
	, m_sceneWorkingDirectory()
	, m_sceneWorkingDirectoryPreview()
	, m_withContainingFolder(true)
{}

void ImguiEditorSceneCreator::buildWindow(
	const char* const title, 
	ImguiEditorUIProxy editorUI,
	bool* const isOpening)
{
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if(!ImGui::Begin(title, isOpening, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	Editor& editor = editorUI.getEditor();

	ImGui::InputText(
		"Scene Name",
		m_sceneNameBuffer.data(),
		m_sceneNameBuffer.size());

	{
		ImguiFileSystemDialog& fsDialog = editorUI.getGeneralFileSystemDialog();
		if(ImGui::Button("Select Scene Working Directory"))
		{
			m_sceneWorkingDirectory = Path();
			m_sceneWorkingDirectoryPreview.clear();
			fsDialog.openPopup(ImguiFileSystemDialog::OPEN_FOLDER_TITLE);
		}

		fsDialog.buildFileSystemDialogPopupModal(
			ImguiFileSystemDialog::OPEN_FOLDER_TITLE,
			editorUI,
			{.canSelectItem = false, .canSelectDirectory = true, .requiresDirectorySelection = true});

		ImGui::Checkbox("With Containing Folder", &m_withContainingFolder);
		ImGui::Text("Directory Preview:");

		//if(fsDialog.selectionConfirmed())
		//{
		//	// Optionally add a folder that has the same name as the scene
		//	if(m_withContainingFolder)
		//	{
		//		Path containingFolder = fsDialog.getSelectedDirectory().getTrailingElement();
		//		if(containingFolder.toString() != m_activeScene->getName())
		//		{
		//			workingDirectory = workingDirectory / m_activeScene->getName();
		//		}
		//	}
		//}
	}
	

	if(ImGui::Button("OK"))
	{
		//editor.createScene(m_newSceneNameBuffer.data());

		if(isOpening)
		{
			*isOpening = false;
		}
	}
	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if(ImGui::Button("Cancel"))
	{
		if(isOpening)
		{
			*isOpening = false;
		}
	}

	ImGui::End();
}

}// end namespace ph::editor
