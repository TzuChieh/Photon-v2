#include "Render/Imgui/Editor/ImguiEditorPropertyPanel.h"
#include "Render/Imgui/Tool/ImguiEditorObjectTypeMenu.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "Render/Imgui/Utility/imgui_helpers.h"
#include "App/Editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"

#include "ThirdParty/DearImGui.h"

#include <cstddef>
#include <utility>
#include <array>
#include <string>

namespace ph::editor
{

ImguiEditorPropertyPanel::ImguiEditorPropertyPanel(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_layoutObjID(EMPTY_SDL_RESOURCE_ID)
	, m_propertyLayout()
	, m_stringEditCache(256, '\0')
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
	if(getEditorUI().getObjectTypeMenu().menuButton(
		PH_IMGUI_PLUS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Child Object ",
		selectedClass))
	{
		// TODO: add child
	}
	if(!canAddChildObj) { ImGui::EndDisabled(); }

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

		// Pop out full object name in case there was not enough space for it
		if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::SetTooltip(primaryObj->getName().c_str());
		}

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
			constexpr ImGuiTableFlags tableFlags =
				ImGuiTableFlags_BordersOuter |
				ImGuiTableFlags_Resizable;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			if(ImGui::BeginTable(group.getName().c_str(), 2, tableFlags))
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
		ImGui::TextUnformatted(prop.getDisplayName().c_str());

		// Pop out additional information such as documentation when hovered
		if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::SetNextWindowSize({ImGui::GetFontSize() * 20.0f, 0.0f});
			if(ImGui::BeginTooltip())
			{
				// Show full property name in case there was not enough space for it
				ImGui::TextUnformatted(prop.getDisplayName().c_str());
				if(!prop.getHelpMessage().empty())
				{
					ImGui::Separator();
					ImGui::TextWrapped(prop.getHelpMessage().c_str());
				}
				ImGui::EndTooltip();
			}
		}

		ImGui::TableSetColumnIndex(1);

		SdlNativeData nativeData = prop.getData();
		if(nativeData.elementContainer == ESdlDataFormat::None || 
		   nativeData.elementType == ESdlDataType::None)
		{
			ImGui::TextUnformatted("(data unavailable)");
			continue;
		}

		if(prop.isReadOnly())
		{
			ImGui::BeginDisabled();
		}

		if(nativeData.elementContainer == ESdlDataFormat::Single)
		{
			switch(nativeData.elementType)
			{
			case ESdlDataType::Bool:
			{
				bool* boolPtr = nativeData.directAccess<bool>();
				if(boolPtr)
				{
					if(nativeData.isNullClearable)
					{
						if(ImGui::Button(PH_IMGUI_CROSS_ICON))
						{
							nativeData.set(0, nullptr);
						}
						ImGui::SameLine();
					}
					ImGui::Checkbox("##prop", boolPtr);
				}
				else
				{
					if(ImGui::Button(PH_IMGUI_PLUS_ICON " Add Boolean "))
					{
						nativeData.set(0, false);
					}
				}
				break;
			}

			case ESdlDataType::Int8:
			case ESdlDataType::UInt8:
			case ESdlDataType::Int16:
			case ESdlDataType::UInt16:
			case ESdlDataType::Int32:
			case ESdlDataType::UInt32:
			case ESdlDataType::Int64:
			case ESdlDataType::UInt64:
			case ESdlDataType::Float32:
			case ESdlDataType::Float64:
			{
				if(nativeData.isIntegerElement())
				{
					std::optional<int64> optValue = nativeData.get<int64>(0);
					if(optValue)
					{
						if(nativeData.isNullClearable)
						{
							if(ImGui::Button(PH_IMGUI_CROSS_ICON))
							{
								nativeData.set(0, nullptr);
							}
							ImGui::SameLine();
						}
						ImGui::SetNextItemWidth(-FLT_MIN);
						const bool isValueChanged = ImGui::InputScalar(
							"##prop",
							ImGuiDataType_S64,
							&(*optValue));
						if(isValueChanged)
						{
							nativeData.set(0, *optValue);
						}
					}
					else
					{
						if(ImGui::Button(PH_IMGUI_PLUS_ICON " Add Value "))
						{
							nativeData.set(0, 0);
						}
					}
				}
				else if(nativeData.isFloatingPointElement())
				{
					std::optional<float64> optValue = nativeData.get<float64>(0);
					if(optValue)
					{
						if(nativeData.isNullClearable)
						{
							if(ImGui::Button(PH_IMGUI_CROSS_ICON))
							{
								nativeData.set(0, nullptr);
							}
							ImGui::SameLine();
						}
						ImGui::SetNextItemWidth(-FLT_MIN);
						const bool isValueChanged = ImGui::InputScalar(
							"##prop",
							ImGuiDataType_Double,
							&(*optValue));
						if(isValueChanged)
						{
							nativeData.set(0, *optValue);
						}
					}
					else
					{
						if(ImGui::Button(PH_IMGUI_PLUS_ICON " Add Value "))
						{
							nativeData.set(0, 0.0);
						}
					}
				}
				else
				{
					ImGui::TextUnformatted("(data unavailable)");
				}
				break;
			}

			case ESdlDataType::String:
			{
				auto strPtr = nativeData.directAccess<std::string>();
				if(strPtr)
				{
					auto resizeCallback = [](ImGuiInputTextCallbackData* cbData) -> int
					{
						if(cbData->EventFlag == ImGuiInputTextFlags_CallbackResize)
						{
							auto stdVec = reinterpret_cast<std::vector<char>*>(cbData->UserData);
							PH_ASSERT(stdVec->data() == cbData->Buf);

							// On resizing calls, generally `cbData->BufSize == cbData->BufTextLen + 1`
							stdVec->resize(cbData->BufSize);

							cbData->Buf = stdVec->data();
						}
						return 0;
					};

					// Copy string into edit cache to display on UI (+1 for null terminator)
					if(strPtr->size() + 1 > m_stringEditCache.size())
					{
						m_stringEditCache.resize(strPtr->size() + 1);
					}
					imgui::copy_to(m_stringEditCache, *strPtr);

					ImGui::SetNextItemWidth(-FLT_MIN);
					if(ImGui::InputText(
						"##prop",
						m_stringEditCache.data(),
						m_stringEditCache.size(),
						ImGuiInputTextFlags_CallbackResize,
						resizeCallback,
						&m_stringEditCache))
					{
						strPtr->assign(m_stringEditCache.data());
					}
				}
				else
				{
					ImGui::TextUnformatted("(string unavailable)");
				}
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
				const auto numElements = static_cast<int>(nativeData.numElements);
				if(numElements > 0)
				{
					if(nativeData.isNullClearable)
					{
						if(ImGui::Button(PH_IMGUI_CROSS_ICON))
						{
							nativeData.set(0, nullptr);
						}

						// For the element tweaker table (exists only if there is any element)
						PH_ASSERT_GT(numElements, 0);
						ImGui::SameLine();
					}
				}
				else
				{
					if(numElements == 0 && ImGui::Button(PH_IMGUI_PLUS_ICON " Add Values "))
					{
						// Trigger value creation by assigning to any slot
						nativeData.set(0, 0);
					}
				}

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
				if(numElements > 0 && ImGui::BeginTable("##prop", numElements))
				{
					ImGui::TableNextRow();
					if(nativeData.isIntegerElement())
					{
						for(int ei = 0; ei < nativeData.numElements; ++ei)
						{
							ImGui::PushID(ei);
							ImGui::PushStyleColor(ImGuiCol_Border, borderColors[ei]);
							ImGui::TableSetColumnIndex(ei);
							ImGui::SetNextItemWidth(-FLT_MIN);
							std::optional<int64> optValue = nativeData.get<int64>(ei);
							if(optValue && ImGui::InputScalar(
								"##prop",
								ImGuiDataType_S64,
								&(*optValue)))
							{
								nativeData.set(ei, *optValue);
							}
							ImGui::PopStyleColor();
							ImGui::PopID();
						}
					}
					else if(nativeData.isFloatingPointElement())
					{
						for(int ei = 0; ei < nativeData.numElements; ++ei)
						{
							ImGui::PushID(ei);
							ImGui::PushStyleColor(ImGuiCol_Border, borderColors[ei]);
							ImGui::TableSetColumnIndex(ei);
							ImGui::SetNextItemWidth(-FLT_MIN);
							std::optional<float64> optValue = nativeData.get<float64>(ei);
							if(optValue && ImGui::InputScalar(
								"##prop",
								ImGuiDataType_Double,
								&(*optValue)))
							{
								nativeData.set(ei, *optValue);
							}
							ImGui::PopStyleColor();
							ImGui::PopID();
						}
					}
					else
					{
						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted("(data unavailable)");
					}
					ImGui::EndTable();
				}
				ImGui::PopStyleVar(3);
				break;
			}

			default:
				ImGui::TextUnformatted("(data unavailable)");
				break;
			}
		}

		if(prop.isReadOnly())
		{
			ImGui::EndDisabled();
		}

		ImGui::PopID();
	}// end for each UI property

	//ImGui::PopStyleVar();
}

}// end namespace ph::editor
