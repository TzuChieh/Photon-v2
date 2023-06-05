#pragma once

#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"

#include <array>
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
	bool m_shouldSaveWithFolder;
	std::array<char, 128> m_newSceneNameBuffer;
};

}// end namespace ph::editor
