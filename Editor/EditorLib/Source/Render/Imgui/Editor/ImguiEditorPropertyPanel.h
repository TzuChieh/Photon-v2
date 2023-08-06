#pragma once

#include "Designer/UI/UIPropertyLayout.h"

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiEditorPropertyPanel final
{
public:
	ImguiEditorPropertyPanel();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

	void setLayout(UIPropertyLayout layout);
	void clearLayout();

private:
	UIPropertyLayout m_layout;
};

}// end namespace ph::editor
