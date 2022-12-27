#pragma once

#include <Common/assertion.h>

namespace ph::editor
{

class Editor;

class DesignerScene final
{
public:
	DesignerScene();
	~DesignerScene();

	// TODO: create typed designer object

	void setEditor(Editor* editor);
	Editor& getEditor();
	const Editor& getEditor() const;

private:
	Editor* m_editor;
};

inline Editor& DesignerScene::getEditor()
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

inline const Editor& DesignerScene::getEditor() const
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

}// end namespace ph::editor
