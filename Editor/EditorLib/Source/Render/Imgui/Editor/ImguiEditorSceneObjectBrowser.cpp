#include "Render/Imgui/Editor/ImguiEditorSceneObjectBrowser.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
//#include "Render/Imgui/Font/IconsMaterialDesign.h"
#include "Render/Imgui/Font/IconsMaterialDesignIcons.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "Render/Imgui/Utility/imgui_helpers.h"
#include "App/Event/ActiveDesignerSceneChangedEvent.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <SDL/Introspect/SdlClass.h>

#include <algorithm>

#define PH_IMGUI_VISIBLE_ICON       ICON_MDI_EYE
#define PH_IMGUI_INVISIBLE_ICON     ICON_MDI_EYE_OFF
#define PH_IMGUI_OBJECT_ICON        ICON_MDI_CUBE
#define PH_IMGUI_ARROW_LEFT_ICON    ICON_MDI_ARROW_LEFT_THICK
#define PH_IMGUI_ARROW_RIGHT_ICON   ICON_MDI_ARROW_RIGHT_THICK
#define PH_IMGUI_EXPAND_ICON        ICON_MDI_PLUS_BOX_OUTLINE

#define PH_IMGUI_OBJECT_ICON_PREFIX PH_IMGUI_OBJECT_ICON " "

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
	: m_activeSceneChanged(nullptr)
	, m_sceneObjectAdded(nullptr)
	, m_sceneObjectRemoval(nullptr)
	, m_objViewLevel(0)
	, m_objViewLevelName()
	, m_expandedObj(nullptr)
	, m_objs()
	, m_isObjsDirty(false)
{
	resetObjectView(nullptr);
}

void ImguiEditorSceneObjectBrowser::initialize(ImguiEditorUIProxy editorUI)
{
	m_activeSceneChanged = editorUI.getEditor().onActiveDesignerSceneChanged
		.addListener<&ImguiEditorSceneObjectBrowser::onActiveSceneChanged>(this);
}

void ImguiEditorSceneObjectBrowser::terminate(ImguiEditorUIProxy editorUI)
{
	editorUI.getEditor().onActiveDesignerSceneChanged.removeListener(m_activeSceneChanged);
	m_activeSceneChanged = nullptr;

	DesignerScene* activeScene = editorUI.getEditor().getActiveScene();
	if(activeScene)
	{
		activeScene->onObjectAdded.removeListener(m_sceneObjectAdded);
		activeScene->onObjectRemoval.removeListener(m_sceneObjectRemoval);
		m_sceneObjectAdded = nullptr;
		m_sceneObjectRemoval = nullptr;
	}
}

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
			buildObjectsContent(scene);
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

void ImguiEditorSceneObjectBrowser::onActiveSceneChanged(const ActiveDesignerSceneChangedEvent& e)
{
	if(e.getDeactivatedScene())
	{
		e.getDeactivatedScene()->onObjectAdded.removeListener(m_sceneObjectAdded);
		e.getDeactivatedScene()->onObjectRemoval.removeListener(m_sceneObjectRemoval);
		m_sceneObjectAdded = nullptr;
		m_sceneObjectRemoval = nullptr;
	}

	if(e.getActivatedScene())
	{
		m_sceneObjectAdded = e.getActivatedScene()->onObjectAdded
			.addListener<&ImguiEditorSceneObjectBrowser::onSceneObjectChanged>(this);
		m_sceneObjectRemoval = e.getActivatedScene()->onObjectRemoval
			.addListener<&ImguiEditorSceneObjectBrowser::onSceneObjectChanged>(this);
	}

	resetObjectView(e.getActivatedScene());
}

void ImguiEditorSceneObjectBrowser::onSceneObjectChanged(const DesignerSceneEvent& e)
{
	m_isObjsDirty = true;
}

void ImguiEditorSceneObjectBrowser::resetObjectView(DesignerScene* const scene)
{
	m_expandedObj = nullptr;
	m_objViewLevel = 0;
	m_isObjsDirty = true;
}

void ImguiEditorSceneObjectBrowser::rebuildObjectView(
	DesignerScene* const scene, 
	const ESortMode sortMode)
{
	if(isViewingRootLevel())
	{
		PH_ASSERT(!m_expandedObj);

		m_objViewLevelName = scene ? "Scene: " + scene->getName() : "Scene: (no active scene)";

		if(scene)
		{
			auto rootObjs = scene->getRootObjects();
			m_objs.resize(rootObjs.size());
			std::copy(rootObjs.begin(), rootObjs.end(), m_objs.begin());
		}
		else
		{
			m_objs.clear();
		}
	}
	else
	{
		PH_ASSERT(m_expandedObj);

		m_objViewLevelName = "Object: " + m_expandedObj->getName();

		auto childObjs = m_expandedObj->getChildren();
		m_objs.resize(childObjs.size());
		std::copy(childObjs.begin(), childObjs.end(), m_objs.begin());
	}

	if(sortMode != ESortMode::None && m_objs.size() > 1)
	{
		switch(sortMode)
		{
		case ESortMode::AscendingName:
			std::sort(m_objs.begin(), m_objs.end(),
				[](const DesignerObject* objA, const DesignerObject* objB)
				{
					return objA->getName() < objB->getName();
				});
			break;

		case ESortMode::DescendingName:
			std::sort(m_objs.begin(), m_objs.end(),
				[](const DesignerObject* objA, const DesignerObject* objB)
				{
					return objA->getName() > objB->getName();
				});
			break;

		// Objects can easily have identical type names, use stable sort to preserve relative order
		case ESortMode::AscendingType:
			std::stable_sort(m_objs.begin(), m_objs.end(),
				[](const DesignerObject* objA, const DesignerObject* objB)
				{
					return objA->getDynamicSdlClass()->getDocName() < objB->getDynamicSdlClass()->getDocName();
				});
			break;

		// Objects can easily have identical type names, use stable sort to preserve relative order
		case ESortMode::DescendingType:
			std::stable_sort(m_objs.begin(), m_objs.end(),
				[](const DesignerObject* objA, const DesignerObject* objB)
				{
					return objA->getDynamicSdlClass()->getDocName() > objB->getDynamicSdlClass()->getDocName();
				});
			break;
		}
	}
}

void ImguiEditorSceneObjectBrowser::buildObjectsContent(DesignerScene* scene)
{
	ImGuiStyle& style = ImGui::GetStyle();

	// Top bar widgets (search, filter, etc.)

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 1));

	if(isViewingRootLevel()) { ImGui::BeginDisabled(); }
	if(ImGui::Button(PH_IMGUI_ARROW_LEFT_ICON))
	{
		// TODO
	}
	if(isViewingRootLevel()) { ImGui::EndDisabled(); }

	ImGui::SameLine();

	const bool canExpand = 
		(scene && scene->getSelection().size() == 1 && scene->getSelection().front()->haveChildren());

	if(!canExpand) { ImGui::BeginDisabled(); }
	if(ImGui::Button(PH_IMGUI_ARROW_RIGHT_ICON))
	{
		// TODO
	}
	if(!canExpand) { ImGui::EndDisabled(); }

	ImGui::PopStyleVar();

	imgui::text_unformatted(m_objViewLevelName);

	constexpr ImGuiTableFlags tableFlags = 
		ImGuiTableFlags_SortTristate |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_BordersV | 
		ImGuiTableFlags_BordersOuterH | 
		ImGuiTableFlags_Resizable | 
		ImGuiTableFlags_RowBg | 
		ImGuiTableFlags_NoBordersInBody;
	constexpr int numColumns = 3;
	constexpr ImGuiID nameColumnId = 0;
	constexpr ImGuiID visibilityColumnId = 1;
	constexpr ImGuiID typeColumnId = 2;
	
	const float textBaseWidth = ImGui::CalcTextSize("A").x;

	if(ImGui::BeginTable("obj_view_table", numColumns, tableFlags))
	{
		// Make title row always visible
		ImGui::TableSetupScrollFreeze(0, 1);

		// The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
		ImGui::TableSetupColumn("Name", 
			ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide,
			0,
			nameColumnId);
		ImGui::TableSetupColumn("", 
			ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed,
			0,
			visibilityColumnId);
		ImGui::TableSetupColumn("Type", 
			ImGuiTableColumnFlags_WidthFixed, 
			textBaseWidth * 18.0f,
			typeColumnId);
		ImGui::TableHeadersRow();

		// Sort object view if sort specs have been changed
		auto sortMode = ESortMode::None;
		ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
		if(sortSpecs && sortSpecs->SpecsDirty)
		{
			if(sortSpecs->SpecsCount == 1)
			{
				const ImGuiTableColumnSortSpecs& spec = sortSpecs->Specs[0];
				if(spec.ColumnUserID == nameColumnId)
				{
					sortMode = spec.SortOrder == ImGuiSortDirection_Ascending
						? ESortMode::AscendingName : ESortMode::DescendingName;
				}
				else if(spec.ColumnUserID == typeColumnId)
				{
					sortMode = spec.SortOrder == ImGuiSortDirection_Ascending
						? ESortMode::AscendingType : ESortMode::DescendingType;
				}
			}

			sortSpecs->SpecsDirty = false;
			m_isObjsDirty = true;
		}

		if(m_isObjsDirty)
		{
			rebuildObjectView(scene, sortMode);
			m_isObjsDirty = false;
		}

		constexpr ImGuiSelectableFlags selectableFlags =
			ImGuiSelectableFlags_SpanAllColumns |
			ImGuiSelectableFlags_AllowItemOverlap;

		const float smallButtonWidth = ImGui::CalcTextSize(PH_IMGUI_EXPAND_ICON).x + style.FramePadding.x * 2.0f;

		// TODO: try PushStyleCompact

		for(DesignerObject* obj : m_objs)
		{
			ImGui::TableNextRow();

			// Object name
			ImGui::TableNextColumn();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, style.ItemSpacing.y));
			if(obj->haveChildren())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
				if(ImGui::SmallButton(PH_IMGUI_EXPAND_ICON))
				{
					// TODO
				}
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::Dummy({smallButtonWidth, 0.0f});
			}
			ImGui::SameLine();
			imgui::text_unformatted(PH_IMGUI_OBJECT_ICON_PREFIX);
			ImGui::SameLine();
			if(ImGui::Selectable(obj->getName().c_str(), obj->isSelected(), selectableFlags))
			{
				PH_ASSERT(scene);
				scene->clearSelection();
				obj->select();
			}
			ImGui::PopStyleVar();

			// Visibility toggle
			ImGui::TableNextColumn();
			buildVisibilityToggle(*obj);

			// Object type
			ImGui::TableNextColumn();
			imgui::text_unformatted(obj->getDynamicSdlClass()->getDocName());
		}

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

}// end namespace ph::editor
