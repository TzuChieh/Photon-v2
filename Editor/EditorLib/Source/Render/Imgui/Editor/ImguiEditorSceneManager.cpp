#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"

#include "ThirdParty/DearImGui.h"

#include <cstddef>

namespace ph::editor
{

ImguiEditorSceneManager::ImguiEditorSceneManager()
{}

void ImguiEditorSceneManager::buildWindow(
	const char* title, 
	Editor& editor,
	bool* isOpening)
{
	if(!ImGui::Begin(title, isOpening))
	{
		ImGui::End();
		return;
	}

	ImGui::Button("New");
	ImGui::SameLine();
	ImGui::Button("Open");

	ImGui::Text("Active Scene: %s", 
		editor.getActiveScene() ? editor.getActiveScene()->getName().c_str() : "(none)");
	
	// List box for all opened scenes
	// Custom size: use all width, 5 items tall
	if(ImGui::BeginListBox("##scenes_listbox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for(std::size_t sceneIdx = 0; sceneIdx < editor.numScenes(); ++sceneIdx)
		{
			DesignerScene* const scene = editor.getScene(sceneIdx);
			const bool isSelected = scene == editor.getActiveScene();
			if(ImGui::Selectable(scene->getName().c_str(), isSelected))
			{
				if(!isSelected)
				{
					editor.setActiveScene(sceneIdx);
				}
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Button("Make Active");
	ImGui::SameLine();
	ImGui::Button("Save");

	ImGui::End();
}

}// end namespace ph::editor
