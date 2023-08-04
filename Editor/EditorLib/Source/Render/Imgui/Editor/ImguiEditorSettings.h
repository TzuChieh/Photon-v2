#pragma once

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiEditorSettings final
{
public:
	ImguiEditorSettings();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

private:
	// Update `CATEGORY_NAMES` is modified
	enum ECategory : int
	{
		Startup = 0
	};

	static constexpr const char* CATEGORY_NAMES[] =
	{
		"Startup"
	};

	void buildCategorySelectionContent();
	void buildSettingsContent();
	void buildStartupCategoryContent();

	int m_category;
};

}// end namespace ph::editor
