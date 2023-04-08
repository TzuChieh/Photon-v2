#pragma once

#include "Designer/ViewportCamera.h"

#include <Common/assertion.h>

namespace ph::editor
{

class Editor;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

class DesignerScene final
{
public:
	DesignerScene();
	~DesignerScene();

	// TODO: create typed designer object

	void update(const MainThreadUpdateContext& ctx);
	void renderUpdate(const MainThreadRenderUpdateContext& ctx);
	void createRenderCommands(RenderThreadCaller& caller);

	void onSceneCreated(Editor* fromEditor);
	void onSceneRemoved();

	Editor& getEditor();
	const Editor& getEditor() const;

private:
	Editor* m_editor;
	ViewportCamera m_mainCamera;
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
