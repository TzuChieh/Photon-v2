#pragma once

namespace ph::editor
{

class Editor;

class ImguiEditorSceneManager final
{
public:
	ImguiEditorSceneManager();

	void buildWindow(
		const char* title, 
		Editor& editor,
		bool* isOpening = nullptr);

private:
};

}// end namespace ph::editor
