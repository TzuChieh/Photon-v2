#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

namespace ph::editor
{

class ImguiEditorAssetBrowser : public ImguiEditorPanel
{
public:
	explicit ImguiEditorAssetBrowser(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	void buildControlPanelContent();
	void buildItemViewContent();

private:
	static constexpr int ORDINARY_ASSET_ONLY = 0;
	static constexpr int CORE_ASSET_ONLY = 1;

	int m_filterState;

	// Ordinary asset visibilities
	bool m_geometryVisibility;
	bool m_materialVisibility;
	bool m_motionVisibility;
	bool m_imageVisibility;

	// Core asset visibilities
	bool m_sampleSourceVisibility;
	bool m_visualizerVisibility;
	bool m_optionVisibility;
};

}// end namespace ph::editor
