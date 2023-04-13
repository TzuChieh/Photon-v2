#pragma once

#include "Designer/designer_fwd.h"
#include "Designer/ViewportCamera.h"

#include <Common/assertion.h>

#include <vector>
#include <memory>

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

	void onCreate(Editor* fromEditor);
	void onRemove();
	void onObjectCreated(DesignerObject* obj);
	void onObjectRemoved(std::shared_ptr<DesignerObject> obj);
	void onObjectTickStateChanged(DesignerObject* obj, bool isTicking);
	void onObjectRenderTickStateChanged(DesignerObject* obj, bool isTicking);

	Editor& getEditor();
	const Editor& getEditor() const;

private:
	std::vector<std::shared_ptr<DesignerObject>> m_rootObjs;
	std::vector<DesignerObject*> m_pendingTickObjs;
	std::vector<DesignerObject*> m_pendingRenderTickObjs;
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

#include "Designer/DesignerScene.ipp"
