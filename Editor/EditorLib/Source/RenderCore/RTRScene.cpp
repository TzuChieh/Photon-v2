#include "RenderCore/RTRScene.h"
#include "RenderCore/CustomRenderContent.h"
#include "RenderCore/RenderThreadUpdateContext.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RTRScene, RenderCore);

RTRScene::~RTRScene() = default;

void RTRScene::update(const RenderThreadUpdateContext& ctx)
{
	for(CustomRenderContent* const customRenderContent : m_customRenderContents)
	{
		customRenderContent->update(ctx);
	}
}

void RTRScene::createGHICommands(GHIThreadCaller& caller)
{
	for(CustomRenderContent* const customRenderContent : m_customRenderContents)
	{
		customRenderContent->createGHICommands(caller);
	}
}

void RTRScene::addCustomRenderContent(CustomRenderContent* const content)
{
	if(!content)
	{
		PH_LOG_WARNING(RTRScene,
			"custom render content ignored since it is empty");
		return;
	}

	m_customRenderContents.push_back(content);
}

void RTRScene::removeCustomRenderContent(CustomRenderContent* const content)
{
	if(!content)
	{
		PH_LOG_WARNING(RTRScene,
			"attempting to remove empty custom render content");
		return;
	}

	const auto numErasedContents = std::erase(m_customRenderContents, content);
	if(numErasedContents != 1)
	{
		PH_LOG_WARNING(RTRScene,
			"on custom render content removal: {}",
			numErasedContents == 0 ? "content not found" : "duplicates found and removed");
	}
}

}// end namespace ph::editor
