#include "RenderCore/RTRScene.h"
#include "RenderCore/CustomRenderContent.h"

namespace ph::editor
{

RTRScene::~RTRScene() = default;

void RTRScene::update(const RenderThreadUpdateContext& ctx)
{
	for(const auto& customRenderContent : customRenderContents)
	{
		customRenderContent->update(ctx);
	}
}

void RTRScene::createGHICommands()
{
	for(const auto& customRenderContent : customRenderContents)
	{
		customRenderContent->createGHICommands();
	}
}

}// end namespace ph::editor
