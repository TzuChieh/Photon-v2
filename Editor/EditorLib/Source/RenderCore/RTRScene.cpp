#include "RenderCore/RTRScene.h"
#include "RenderCore/RenderThreadUpdateContext.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RTRScene, RenderCore);

RTRScene::~RTRScene() = default;

void RTRScene::update(const RenderThreadUpdateContext& ctx)
{
	for(auto& customRenderContent : m_customRenderContents)
	{
		customRenderContent->update(ctx);
	}
}

void RTRScene::createGHICommands(GHIThreadCaller& caller)
{
	for(auto& customRenderContent : m_customRenderContents)
	{
		customRenderContent->createGHICommands(caller);
	}
}

void RTRScene::addCustomRenderContent(std::unique_ptr<CustomRenderContent> content)
{
	if(!content)
	{
		PH_LOG_WARNING(RTRScene,
			"custom render content ignored since it is empty");
		return;
	}

	m_customRenderContents.add(std::move(content));
}

void RTRScene::removeCustomRenderContent(CustomRenderContent* const content)
{
	if(!content)
	{
		PH_LOG_WARNING(RTRScene,
			"attempting to remove empty custom render content");
		return;
	}

	auto removedContent = m_customRenderContents.remove(content);
	if(!removedContent)
	{
		PH_LOG_WARNING(RTRScene,
			"on custom render content removal: did not find specified content, nothing removed");
	}
}

}// end namespace ph::editor
