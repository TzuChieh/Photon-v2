#include "Render/Imgui/Editor/ImguiEditorSceneObjectBrowser.h"
#include "Render/Imgui/Tool/ImguiEditorObjectTypeMenu.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "Render/Imgui/Utility/imgui_helpers.h"
#include "App/Event/ActiveDesignerSceneChangedEvent.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <SDL/Introspect/SdlClass.h>
#include <Utility/string_utils.h>

#include <algorithm>

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

inline std::string_view get_display_type_name(const DesignerObject* obj)
{
	if(!obj)
	{
		return "(null)";
	}

	std::string_view docName = obj->getDynamicSdlClass()->getDocName();

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

	return docName;
}

}// end anonymous namespace

ImguiEditorSceneObjectBrowser::ImguiEditorSceneObjectBrowser(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_activeSceneChanged(nullptr)
	, m_sceneObjectAdded(nullptr)
	, m_sceneObjectRemoval(nullptr)
	, m_objViewLevel(0)
	, m_objViewLevelName()
	, m_expandedObj(nullptr)
	, m_objInfos()
	, m_isObjsDirty(false)
{
	resetObjectView(nullptr);

	m_activeSceneChanged = editorUI.getEditor().onActiveDesignerSceneChanged
		.addListener<&ImguiEditorSceneObjectBrowser::onActiveSceneChanged>(this);
}

ImguiEditorSceneObjectBrowser::~ImguiEditorSceneObjectBrowser()
{
	getEditorUI().getEditor().onActiveDesignerSceneChanged.removeListener(m_activeSceneChanged);
	m_activeSceneChanged = nullptr;

	DesignerScene* activeScene = getEditorUI().getEditor().getActiveScene();
	if(activeScene)
	{
		activeScene->onObjectAdded.removeListener(m_sceneObjectAdded);
		activeScene->onObjectRemoval.removeListener(m_sceneObjectRemoval);
		m_sceneObjectAdded = nullptr;
		m_sceneObjectRemoval = nullptr;
	}
}

void ImguiEditorSceneObjectBrowser::buildWindow(const char* windowIdName, bool* isOpening)
{
	constexpr ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_None;

	if(!ImGui::Begin(windowIdName, isOpening, windowFlags))
	{
		ImGui::End();
		return;
	}

	Editor& editor = getEditorUI().getEditor();
	DesignerScene* scene = editor.getActiveScene();
	if(!scene)
	{
		ImGui::Text("(no active scene)");
		ImGui::End();
		return;
	}

	constexpr ImGuiTabBarFlags tabBarFlags = 
		ImGuiTabBarFlags_FittingPolicyResizeDown;
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

auto ImguiEditorSceneObjectBrowser::getAttributes() const
-> Attributes
{
	return {
		.title = "Object Browser",
		.icon = PH_IMGUI_OBJECTS_ICON,
		.preferredDockingLot = EImguiPanelDockingLot::UpperRight,
		.isOpenedByDefault = true,
		.isCloseable = false};
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
			m_objInfos.resize(rootObjs.size());
			for(std::size_t oi = 0; oi < rootObjs.size(); ++oi)
			{
				m_objInfos[oi].obj = rootObjs[oi];
				m_objInfos[oi].typeName = get_display_type_name(rootObjs[oi]);
			}
		}
		else
		{
			m_objInfos.clear();
		}
	}
	else
	{
		PH_ASSERT(m_expandedObj);

		m_objViewLevelName = "Object: " + m_expandedObj->getName();

		auto childObjs = m_expandedObj->getChildren();
		m_objInfos.resize(childObjs.size());
		for(std::size_t oi = 0; oi < childObjs.size(); ++oi)
		{
			m_objInfos[oi].obj = childObjs[oi];
			m_objInfos[oi].typeName = get_display_type_name(childObjs[oi]);
		}
	}

	if(sortMode != ESortMode::None && m_objInfos.size() > 1)
	{
		switch(sortMode)
		{
		case ESortMode::AscendingName:
			std::sort(m_objInfos.begin(), m_objInfos.end(),
				[](const ObjectInfo& a, const ObjectInfo& b)
				{
					return a.obj->getName() < b.obj->getName();
				});
			break;

		case ESortMode::DescendingName:
			std::sort(m_objInfos.begin(), m_objInfos.end(),
				[](const ObjectInfo& a, const ObjectInfo& b)
				{
					return a.obj->getName() > b.obj->getName();
				});
			break;

		// Objects can easily have identical type names, use stable sort to preserve relative order
		case ESortMode::AscendingType:
			std::stable_sort(m_objInfos.begin(), m_objInfos.end(),
				[](const ObjectInfo& a, const ObjectInfo& b)
				{
					return a.typeName < b.typeName;
				});
			break;

		// Objects can easily have identical type names, use stable sort to preserve relative order
		case ESortMode::DescendingType:
			std::stable_sort(m_objInfos.begin(), m_objInfos.end(),
				[](const ObjectInfo& a, const ObjectInfo& b)
				{
					return a.typeName > b.typeName;
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

	ImGui::SameLine();

	if(!scene) { ImGui::BeginDisabled(); }
	const SdlClass* selectedClass = nullptr;
	getEditorUI().getObjectTypeMenu().buildMenuButton(
		PH_IMGUI_PLUS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Root Object ",
		selectedClass);
	if(!scene) { ImGui::EndDisabled(); }
	if(scene && selectedClass)
	{
		scene->newRootObject(selectedClass);
	}

	imgui::text_unformatted(m_objViewLevelName);

	constexpr ImGuiTableFlags tableFlags = 
		ImGuiTableFlags_Sortable |
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
			ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_NoHide,
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
				if(spec.ColumnUserID == nameColumnId && spec.SortDirection != ImGuiSortDirection_None)
				{
					sortMode = spec.SortDirection == ImGuiSortDirection_Ascending
						? ESortMode::AscendingName : ESortMode::DescendingName;
				}
				else if(spec.ColumnUserID == typeColumnId && spec.SortDirection != ImGuiSortDirection_None)
				{
					sortMode = spec.SortDirection == ImGuiSortDirection_Ascending
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

		for(const ObjectInfo& objInfo : m_objInfos)
		{
			DesignerObject* obj = objInfo.obj;

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
			imgui::text_unformatted(objInfo.typeName);
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
