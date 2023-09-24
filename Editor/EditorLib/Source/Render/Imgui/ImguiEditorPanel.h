#pragma once

#include "Render/Imgui/ImguiEditorUIProxy.h"

#include <string>

namespace ph::editor
{

enum EImguiPanelCategory
{
	Window = 0,
	File,
	Edit,
	Tools,
	Debug
};

enum EImguiPanelDockingLot
{
	None = 0,
	Center,
	Bottom,
	UpperRight,
	LowerRight
};

class ImguiEditorPanel
{
public:
	struct Attributes
	{
		std::string title;
		std::string icon;
		std::string tooltip;
		EImguiPanelCategory category = static_cast<EImguiPanelCategory>(0);
		EImguiPanelDockingLot preferredDockingLot = static_cast<EImguiPanelDockingLot>(0);
		bool isOpenedByDefault = false;
		bool isCloseable = true;
		bool useMenubar = true;
		bool useSidebar = false;
	};

	explicit ImguiEditorPanel(ImguiEditorUIProxy editorUI);
	virtual ~ImguiEditorPanel();

	virtual void buildWindow(const char* windowIdName, bool* isOpening) = 0;
	virtual auto getAttributes() const -> Attributes = 0;

	ImguiEditorUIProxy getEditorUI() const;

private:
	ImguiEditorUIProxy m_editorUI;
};

inline ImguiEditorUIProxy ImguiEditorPanel::getEditorUI() const
{
	return m_editorUI;
}

}// end namespace ph::editor
