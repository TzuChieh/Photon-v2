#include "Designer/DesignerScene.h"
#include "App/Editor.h"

namespace ph::editor
{

DesignerScene::DesignerScene()
	: m_editor(nullptr)
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

void DesignerScene::onSceneCreated(Editor* const fromEditor)
{
	m_editor = fromEditor;
}

void DesignerScene::onSceneRemoved()
{
	m_editor = nullptr;
}

}// end namespace ph::editor
