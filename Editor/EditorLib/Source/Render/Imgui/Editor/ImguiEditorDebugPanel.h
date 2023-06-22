#pragma once

#include <vector>

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiEditorDebugPanel final
{
public:
	ImguiEditorDebugPanel();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

private:
	std::vector<char> m_objectNameBuffer;
};

}// end namespace ph::editor
