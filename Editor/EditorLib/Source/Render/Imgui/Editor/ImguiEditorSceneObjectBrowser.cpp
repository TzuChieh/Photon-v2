#include "Render/Imgui/Editor/ImguiEditorSceneObjectBrowser.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
//#include "Render/Imgui/Font/IconsMaterialDesign.h"
#include "Render/Imgui/Font/IconsMaterialDesignIcons.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <SDL/Introspect/SdlClass.h>

#define PH_IMGUI_VISIBLE_ICON   ICON_MDI_EYE
#define PH_IMGUI_INVISIBLE_ICON ICON_MDI_EYE_OFF
#define PH_IMGUI_OBJECT_ICON    ICON_MDI_CUBE

namespace ph::editor
{

namespace
{

inline const char* get_visibility_icon(const bool isVisible)
{
	return isVisible ? PH_IMGUI_VISIBLE_ICON : PH_IMGUI_INVISIBLE_ICON;
}

inline bool visibility_toggle_button(const char* const strId, const bool isVisible)
{
	// When using the same icon font for the button, they will have the same ID and conflict. 
	// Use label for unique ID.
	ImGui::PushID(strId);

	bool isClicked = false;
	if(ImGui::SmallButton(get_visibility_icon(isVisible)))
	{
		isClicked = true;
	}

	ImGui::PopID();

	return isClicked;
}

}// end anonymous namespace

ImguiEditorSceneObjectBrowser::ImguiEditorSceneObjectBrowser()
	: m_displayNameBuffer(128, '\0')
{}

void ImguiEditorSceneObjectBrowser::buildWindow(
	const char* title, 
	ImguiEditorUIProxy editorUI,
	bool* isOpening)
{
	constexpr ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_None;

	if(!ImGui::Begin(title, isOpening, windowFlags))
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
	constexpr ImGuiTableFlags tableFlags = 
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_BordersV | 
		ImGuiTableFlags_BordersOuterH | 
		ImGuiTableFlags_Resizable | 
		ImGuiTableFlags_RowBg | 
		ImGuiTableFlags_NoBordersInBody;

	constexpr int numColumns = 3;
	const float textBaseWidth = ImGui::CalcTextSize("A").x;

	if(ImGui::BeginTable("obj_view_table", numColumns, tableFlags))
	{
		// Make title row always visible
		ImGui::TableSetupScrollFreeze(0, 1);

		// The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, textBaseWidth * 18.0f);
		ImGui::TableHeadersRow();

		for(DesignerObject* rootObj : scene.getRootObjects())
		{
			if(!rootObj)
			{
				continue;
			}

			buildObjectTableRowRecursive(*rootObj);
		}

		// Simple storage to output a dummy file-system.
		struct MyTreeNode
		{
			const char* Name;
			const char* Type;
			int             Size;
			int             ChildIdx;
			int             ChildCount;
			static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				const bool is_folder = (node->ChildCount > 0);
				if(is_folder)
				{
					bool open = ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_SpanFullWidth);
					ImGui::TableNextColumn();
					ImGui::TextDisabled("--");
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(node->Type);
					if(open)
					{
						for(int child_n = 0; child_n < node->ChildCount; child_n++)
							DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
					ImGui::TableNextColumn();
					ImGui::Text("%d", node->Size);
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(node->Type);
				}
			}
		};
		static const MyTreeNode nodes[] =
		{
			{ "Root",                         "Folder",       -1,       1, 3    }, // 0
			{ "Music",                        "Folder",       -1,       4, 2    }, // 1
			{ "Textures",                     "Folder",       -1,       6, 3    }, // 2
			{ "desktop.ini",                  "System file",  1024,    -1,-1    }, // 3
			{ "File1_a.wav",                  "Audio file",   123000,  -1,-1    }, // 4
			{ "File1_b.wav",                  "Audio file",   456000,  -1,-1    }, // 5
			{ "Image001.png",                 "Image file",   203128,  -1,-1    }, // 6
			{ "Copy of Image001.png",         "Image file",   203256,  -1,-1    }, // 7
			{ "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
		};

		MyTreeNode::DisplayNode(&nodes[0], nodes);

		ImGui::EndTable();
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
	const auto numSelectedObjs = static_cast<int>(scene.getSelection().size());
	const auto numAllocatedObjs = static_cast<int>(scene.numAllocatedObjects());

	ImGui::Text("Root: %d", numRootObjs);
	ImGui::Text("Ticking: %d", numTickingObjs);
	ImGui::Text("Render Ticking: %d", numRenderTickingObjs);
	ImGui::Text("Selected: %d", numSelectedObjs);

	ImGui::Separator();

	ImGui::TextUnformatted("Details:");
	ImGui::Text("Allocated: %d", numAllocatedObjs);
}

void ImguiEditorSceneObjectBrowser::buildObjectTableRowRecursive(DesignerObject& obj)
{
	const bool haveChildren = obj.haveChildren();

	ImGui::TableNextRow();

	if(haveChildren)
	{
		constexpr ImGuiTreeNodeFlags nodeFlags =
			ImGuiTreeNodeFlags_SpanFullWidth;

		// Object name
		ImGui::TableNextColumn();
		const bool isObjOpened = ImGui::TreeNodeEx(obj.getName().c_str(), nodeFlags);
			
		// Visibility toggle
		ImGui::TableNextColumn();
		buildVisibilityToggle(obj);

		// Object type
		ImGui::TableNextColumn();
		imgui::text_unformatted(obj.getDynamicSdlClass()->getDocName());

		if(isObjOpened)
		{
			for(DesignerObject* childObj : obj.getChildren())
			{
				buildObjectTableRowRecursive(obj);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		constexpr ImGuiTreeNodeFlags nodeFlags =
			ImGuiTreeNodeFlags_Leaf | 
			ImGuiTreeNodeFlags_NoTreePushOnOpen | 
			ImGuiTreeNodeFlags_SpanFullWidth;

		// Object name
		ImGui::TableNextColumn();
		ImGui::TreeNodeEx(obj.getName().c_str(), nodeFlags);
		// Note: Do not pop leaf node, see: https://github.com/ocornut/imgui/issues/4833
			
		// Visibility toggle
		ImGui::TableNextColumn();
		buildVisibilityToggle(obj);

		// Object type
		ImGui::TableNextColumn();
		imgui::text_unformatted(obj.getDynamicSdlClass()->getDocName());
	}
}

void ImguiEditorSceneObjectBrowser::buildVisibilityToggle(DesignerObject& obj)
{
	if(visibility_toggle_button(obj.getName().c_str(), obj.isVisible()))
	{
		if(obj.isVisible())
		{
			obj.setVisibility(false);
		}
		else
		{
			obj.setVisibility(true);
		}
	}
}

//const char* ImguiEditorSceneObjectBrowser::getObjectDisplayName(
//	std::string_view objName,
//	std::string_view namePrefix,
//	const std::size_t maxChars)
//{
//	prepareDisplayNameBuffer(maxChars);
//
//	if(objName.size() + namePrefix.size() <= maxChars)
//	{
//
//	}
//}

void ImguiEditorSceneObjectBrowser::prepareDisplayNameBuffer(const std::size_t maxChars)
{
	const auto numCharsWithNullTerminator = maxChars + 1;
	if(numCharsWithNullTerminator > m_displayNameBuffer.size())
	{
		m_displayNameBuffer.resize(numCharsWithNullTerminator);
	}
}

}// end namespace ph::editor
