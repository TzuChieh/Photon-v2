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

	ImGui::Text("Active Scene");

	// Custom size: use all width, 5 items tall
	if(ImGui::BeginListBox("##Active Scene", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
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

	ImGui::End();
}

}// end namespace ph::editor
