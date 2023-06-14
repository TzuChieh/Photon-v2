#pragma once

#include <cstddef>

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiEditorSceneManager final
{
public:
	ImguiEditorSceneManager();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

private:
	std::size_t m_selectedSceneIdx;
};

}// end namespace ph::editor
