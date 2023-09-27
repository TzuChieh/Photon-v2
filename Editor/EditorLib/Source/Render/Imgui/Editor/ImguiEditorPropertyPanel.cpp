#include "Render/Imgui/Editor/ImguiEditorPropertyPanel.h"
#include "Render/Imgui/Tool/ImguiEditorObjectTypeMenu.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"

#include "ThirdParty/DearImGui.h"

#include <cstddef>
#include <utility>
#include <string>
#include <array>

namespace ph::editor
{

ImguiEditorPropertyPanel::ImguiEditorPropertyPanel(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_layoutObjID(EMPTY_SDL_RESOURCE_ID)
	, m_propertyLayout()
{}

void ImguiEditorPropertyPanel::buildWindow(const char* windowIdName, bool* isOpening)
{
	constexpr ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_AlwaysVerticalScrollbar;// so width can be fixed--no scrollbar popping

	if(!ImGui::Begin(windowIdName, isOpening, windowFlags))
	{
		ImGui::End();
		return;
	}

	DesignerScene* activeScene = getEditorUI().getEditor().getActiveScene();
	DesignerObject* primaryObj = activeScene ? activeScene->getPrimarySelectedObject() : nullptr;

	const bool canAddChildObj = primaryObj && primaryObj->canHaveChildren();

	if(!canAddChildObj) { ImGui::BeginDisabled(); }
	const SdlClass* selectedClass = nullptr;
	getEditorUI().getObjectTypeMenu().buildMenuButton(
		PH_IMGUI_PLUS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Child Object ", 
		selectedClass);
	if(!canAddChildObj) { ImGui::EndDisabled(); }

	// TODO: add child

	if(!primaryObj)
	{
		ImGui::SameLine();
		ImGui::TextUnformatted("No selected object.");
	}
	else
	{
		ImGui::SameLine();
		ImGui::TextUnformatted("Object:");
		ImGui::SameLine();
		ImGui::TextUnformatted(primaryObj->getName().c_str());

		const std::size_t numSelectedObjs = activeScene ? activeScene->getSelection().size() : 0;
		if(numSelectedObjs > 1)
		{
			ImGui::Text("%d objects were selected. Showing primary.", static_cast<int>(numSelectedObjs));
		}

		buildPropertyEditor(*primaryObj);
	}

	ImGui::End();
}

auto ImguiEditorPropertyPanel::getAttributes() const
-> Attributes
{
	return {
		.title = "Properties",
		.icon = PH_IMGUI_PROPERTIES_ICON,
		.tooltip = "Properties",
		.preferredDockingLot = EImguiPanelDockingLot::LowerRight,
		.isOpenedByDefault = true,
		.isCloseable = false};
}

void ImguiEditorPropertyPanel::buildPropertyEditor(DesignerObject& obj)
{
	if(obj.getId() != m_layoutObjID)
	{
		m_propertyLayout.clear();

		m_propertyLayout = obj.layoutProperties();
		m_layoutObjID = obj.getId();
	}

	int groupID = 0;
	for(const UIPropertyGroup& group : m_propertyLayout)
	{
		ImGui::PushID(groupID++);

		if(ImGui::CollapsingHeader(group.getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			if(ImGui::BeginTable(group.getName().c_str(), 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
			{
				buildPropertiesInGroup(group);
				ImGui::EndTable();
			}
			ImGui::PopStyleVar();
		}

		ImGui::PopID();
	}
}

void ImguiEditorPropertyPanel::buildPropertiesInGroup(const UIPropertyGroup& group)
{
	const ImGuiStyle& style = ImGui::GetStyle();
	//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 1));

	int propID = 0;
	for(const UIProperty& prop : group)
	{
		ImGui::PushID(propID++);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		// Text and tree nodes are less high than framed widgets, using `AlignTextToFramePadding()` we
		// add vertical spacing to make the tree lines equal high.
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted( prop.getDisplayName().c_str());

		ImGui::TableSetColumnIndex(1);

		SdlNativeData nativeData = prop.getData();
		if(nativeData.elementContainer == ESdlDataFormat::None)
		{
			ImGui::TextUnformatted("(data unavailable)");
			continue;
		}

		if(nativeData.elementContainer == ESdlDataFormat::Single)
		{
			switch(nativeData.elementType)
			{
			case ESdlDataType::Bool:
			{
				ImGui::Checkbox("##prop", nativeData.directAccess<bool>());
				break;
			}

			case ESdlDataType::String:
			{
				auto str = nativeData.directAccess<std::string>();
				ImGui::TextUnformatted(str ? str->c_str() : "(string unavailable)");
				break;
			}

			default:
				ImGui::TextUnformatted("(data unavailable)");
				break;
			}
		}
		else
		{
			PH_ASSERT(nativeData.elementContainer != ESdlDataFormat::None);
			PH_ASSERT(nativeData.elementContainer != ESdlDataFormat::Single);

			switch(nativeData.elementContainer)
			{
			case ESdlDataFormat::Vector2:
			case ESdlDataFormat::Vector3:
			case ESdlDataFormat::Quaternion:
			{
				constexpr ImVec2 cellPadding = {2, 0};
				constexpr float frameRounding = 3.0f;
				constexpr float frameBorderSize = 1.5;

				constexpr std::array<ImVec4, 4> borderColors = {
					ImVec4{0.8f, 0.3f, 0.3f, 1.0f},
					ImVec4{0.3f, 0.8f, 0.3f, 1.0f},
					ImVec4{0.3f, 0.3f, 0.8f, 1.0f},
					ImVec4{0.8f, 0.8f, 0.8f, 1.0f}};

				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, frameRounding);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, frameBorderSize);
				if(ImGui::BeginTable("##prop", static_cast<int>(nativeData.numElements)))
				{
					ImGui::TableNextRow();
					if(nativeData.isIntegerElement())
					{
						for(std::size_t ei = 0; ei < nativeData.numElements; ++ei)
						{
							ImGui::PushID(static_cast<int>(ei));

							auto value = *nativeData.get<int64>(ei);

							ImGui::PushStyleColor(ImGuiCol_Border, borderColors[ei]);
							ImGui::TableSetColumnIndex(static_cast<int>(ei));
							ImGui::SetNextItemWidth(-FLT_MIN);
							const bool isValueChanged = ImGui::InputScalar(
								"##prop",
								ImGuiDataType_S64,
								&value);
							ImGui::PopStyleColor();

							if(isValueChanged)
							{
								nativeData.set(ei, value);
							}

							ImGui::PopID();
						}
					}
					else if(nativeData.isFloatingPointElement())
					{
						for(std::size_t ei = 0; ei < nativeData.numElements; ++ei)
						{
							ImGui::PushID(static_cast<int>(ei));

							auto value = *nativeData.get<float64>(ei);

							ImGui::PushStyleColor(ImGuiCol_Border, borderColors[ei]);
							ImGui::TableSetColumnIndex(static_cast<int>(ei));
							ImGui::SetNextItemWidth(-FLT_MIN);
							const bool isValueChanged = ImGui::InputScalar(
								"##prop",
								ImGuiDataType_Double,
								&value);
							ImGui::PopStyleColor();

							if(isValueChanged)
							{
								nativeData.set(ei, value);
							}

							ImGui::PopID();
						}
					}
					else
					{
						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted("(data unavailable)");
					}
					ImGui::PopStyleVar(3);
					ImGui::EndTable();
				}

				break;
			}

			default:
				ImGui::TextUnformatted("(data unavailable)");
				break;
			}
		}

		ImGui::PopID();
	}

	//ImGui::PopStyleVar();
}

}// end namespace ph::editor
