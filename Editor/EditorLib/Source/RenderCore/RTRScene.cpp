#include "RenderCore/RTRScene.h"
#include "RenderCore/CustomRenderContent.h"
#include "RenderCore/RenderThreadUpdateContext.h"

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

void RTRScene::createGHICommands(GHIThreadCaller& caller)
{
	for(const auto& customRenderContent : customRenderContents)
	{
		customRenderContent->createGHICommands(caller);
	}
}

}// end namespace ph::editor
