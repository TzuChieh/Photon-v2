#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

#include <vector>

namespace ph::editor
{

class EditorSettings;

class ImguiEditorSettings : public ImguiEditorPanel
{
public:
	explicit ImguiEditorSettings(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	// Update `CATEGORY_NAMES` is modified
	enum ECategory : int
	{
		General = 0,
		Startup,
		Control
	};

	static constexpr const char* CATEGORY_NAMES[] =
	{
		"General",
		"Startup",
		"Control"
	};

	void buildCategorySelectionContent();
	void buildSettingsContent();
	void buildGeneralCategoryContent();
	void buildStartupCategoryContent();
	void buildControlCategoryContent();

	int m_category;
	std::vector<char> m_stringDisplayBuffer;
};

}// end namespace ph::editor
