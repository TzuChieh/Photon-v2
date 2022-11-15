#pragma once

namespace ph::editor
{

class Editor;

class ImguiEditorUI final
{
public:
	ImguiEditorUI();

	void build();

	void setEditor(Editor* editor);

private:
	Editor* m_editor;
};

}// end namespace ph::editor
