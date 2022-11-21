#pragma once

namespace ph::editor
{

class Editor;
class ImguiHelper;

class ImguiEditorUI final
{
public:
	ImguiEditorUI();

	void initialize(Editor* editor, ImguiHelper* helper);
	void build();

private:
	Editor* m_editor;
	ImguiHelper* m_helper;
};

}// end namespace ph::editor
