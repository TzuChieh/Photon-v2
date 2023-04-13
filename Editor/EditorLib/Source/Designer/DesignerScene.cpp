#include "Designer/DesignerScene.h"
#include "App/Editor.h"
#include "Designer/DesignerObject.h"

namespace ph::editor
{

DesignerScene::DesignerScene()
	: m_rootObjs()
	, m_pendingTickObjs()
	, m_pendingRenderTickObjs()
	, m_editor(nullptr)
	, m_mainCamera()
{}

DesignerScene::~DesignerScene() = default;

void DesignerScene::update(const MainThreadUpdateContext& ctx)
{
	// TODO
}

void DesignerScene::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	// TODO
}

void DesignerScene::createRenderCommands(RenderThreadCaller& caller)
{
	// TODO
}

void DesignerScene::onCreate(Editor* const fromEditor)
{
	m_editor = fromEditor;
}

void DesignerScene::onRemove()
{
	m_editor = nullptr;
}

void DesignerScene::onObjectCreated(DesignerObject* const obj)
{
	PH_ASSERT(obj);
	obj->onCreate(this);
}

}// end namespace ph::editor
