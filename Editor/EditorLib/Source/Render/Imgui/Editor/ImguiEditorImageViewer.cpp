#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "App/Editor.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"

#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

ImguiEditorImageViewer::ImguiEditorImageViewer(ImguiEditorUIProxy editorUI)

	: ImguiEditorPanel(editorUI)

	, m_imageNames()
	, m_currentImageNameIdx(static_cast<std::size_t>(-1))
{}

ImguiEditorImageViewer::~ImguiEditorImageViewer()
{}

void ImguiEditorImageViewer::buildWindow(const char* windowIdName, bool* isOpening)
{
	if(!ImGui::Begin(windowIdName, isOpening))
	{
		ImGui::End();
		return;
	}

	ImVec2 imageDisplayAreaMin = ImGui::GetWindowContentRegionMin();
	ImVec2 imageDisplayAreaMax = ImGui::GetWindowContentRegionMax();

	ImguiImageLibrary& imageLib = getEditorUI().getImageLibrary();
	ImguiFileSystemDialog& fsDialog = getEditorUI().getGeneralFileSystemDialog();

	if(m_currentImageNameIdx < m_imageNames.size())
	{
		imageLib.imguiImage(m_imageNames[m_currentImageNameIdx].c_str(), {300, 300});
	}

	if(ImGui::BeginCombo(
		"##image_names_combo", 
		m_currentImageNameIdx < m_imageNames.size() ? m_imageNames[m_currentImageNameIdx].c_str() : "(no loaded image)"))
	{
		for(std::size_t ni = 0; ni < m_imageNames.size(); ++ni)
		{
			const bool isSelected = (ni == m_currentImageNameIdx);
			if(ImGui::Selectable(m_imageNames[ni].c_str(), isSelected))
			{
				m_currentImageNameIdx = ni;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if(ImGui::Button(PH_IMGUI_OPEN_FILE_ICON))
	{
		fsDialog.openPopup(ImguiFileSystemDialog::OPEN_FILE_TITLE);
	}

	ImGui::SameLine();

	if(ImGui::Button(PH_IMGUI_CROSS_ICON))
	{
		// TODO
	}

	fsDialog.buildFileSystemDialogPopupModal(
		ImguiFileSystemDialog::OPEN_FILE_TITLE,
		getEditorUI());

	if(fsDialog.dialogClosed())
	{
		Path imageFile = fsDialog.getSelectedTarget();
		auto imageName = imageFile.toAbsoluteString();
		if(!imageFile.isEmpty() && !imageLib.has(imageName))
		{
			imageLib.loadImage(imageName, imageFile);
			m_imageNames.push_back(imageName);
		}
	}

	ImGui::End();
}

auto ImguiEditorImageViewer::getAttributes() const
-> Attributes
{
	return {
		.title = "Image Viewer",
		.icon = PH_IMGUI_IMAGE_ICON,
		.tooltip = "Image Viewer",
		.preferredDockingLot = EImguiPanelDockingLot::Center,
		.useSidebar = true};
}

}// end namespace ph::editor
