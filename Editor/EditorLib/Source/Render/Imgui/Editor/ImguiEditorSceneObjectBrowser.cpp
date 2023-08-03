#include "Render/Imgui/Editor/ImguiEditorSceneObjectBrowser.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"

#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

ImguiEditorSceneObjectBrowser::ImguiEditorSceneObjectBrowser()
{}

void ImguiEditorSceneObjectBrowser::buildWindow(
	const char* title, 
	ImguiEditorUIProxy editorUI,
	bool* isOpening)
{
	if(!ImGui::Begin(title, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = editorUI.getEditor();
	DesignerScene* scene = editor.getActiveScene();
	if(!scene)
	{
		ImGui::Text("(no active scene)");
		ImGui::End();
		return;
	}

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if(ImGui::BeginTabBar("options_tab_bar", tabBarFlags))
	{
		if(ImGui::BeginTabItem("Objects"))
		{
			buildObjectsContent(*scene);
			ImGui::EndTabItem();
		}
		if(ImGui::BeginTabItem("Filters"))
		{
			buildFiltersContent(*scene);
			ImGui::EndTabItem();
		}
		if(ImGui::BeginTabItem("Stats"))
		{
			buildStatsContent(*scene);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ImguiEditorSceneObjectBrowser::buildObjectsContent(DesignerScene& scene)
{
	ImGuiTreeNodeFlags rootObjNodeFlags = 0;
	rootObjNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

	ImGuiTreeNodeFlags objNodeFlags = 0;

	if(ImGui::TreeNodeEx(scene.getName().c_str(), rootObjNodeFlags))
	{
		for(auto rootObj : scene.getRootObjects())
		{
			if(!rootObj)
			{
				continue;
			}

			if(ImGui::TreeNodeEx(rootObj->getName().c_str(), objNodeFlags))
			{
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}

void ImguiEditorSceneObjectBrowser::buildFiltersContent(DesignerScene& scene)
{
	ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
}

void ImguiEditorSceneObjectBrowser::buildStatsContent(DesignerScene& scene)
{
	const auto numRootObjs = static_cast<int>(scene.numRootObjects());
	const auto numTickingObjs = static_cast<int>(scene.numTickingObjects());
	const auto numRenderTickingObjs = static_cast<int>(scene.numRenderTickingObjects());
	const auto numAllocatedObjs = static_cast<int>(scene.numAllocatedObjects());

	ImGui::Text("Root: %d", numRootObjs);
	ImGui::Text("Ticking: %d", numTickingObjs);
	ImGui::Text("Render Ticking: %d", numRenderTickingObjs);
	ImGui::Text("Allocated: %d", numAllocatedObjs);
}

}// end namespace ph::editor
