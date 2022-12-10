#pragma once

namespace ph::editor
{

class Editor;

class DesignerScene final
{
public:
	~DesignerScene();

	void setEditor(Editor* editor);

	Editor& getEditor();
	const Editor& getEditor() const;

private:
	Editor* m_editor;
};

}// end namespace ph::editor
