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
};

}// end namespace ph::editor
