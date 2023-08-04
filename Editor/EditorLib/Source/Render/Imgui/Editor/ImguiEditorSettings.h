#pragma once

#include <vector>

namespace ph::editor
{

class ImguiEditorUIProxy;
class EditorSettings;

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
		General = 0,
		Startup
	};

	static constexpr const char* CATEGORY_NAMES[] =
	{
		"General",
		"Startup"
	};

	void buildCategorySelectionContent();
	void buildSettingsContent(ImguiEditorUIProxy editorUI);
	void buildGeneralCategoryContent(ImguiEditorUIProxy editorUI);
	void buildStartupCategoryContent(ImguiEditorUIProxy editorUI);

	int m_category;
	std::vector<char> m_stringDisplayBuffer;
};

}// end namespace ph::editor
