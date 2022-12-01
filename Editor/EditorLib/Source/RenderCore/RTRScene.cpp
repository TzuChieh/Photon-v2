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
	for(RTRResource* const resource : m_resourcesPendingSetup)
	{
		resource->setupGHI(caller);
	}
	m_resourcesPendingSetup.clear();

	// TODO: update dynamic resource

	for(auto& customRenderContent : m_customRenderContents)
	{
		customRenderContent->createGHICommands(caller);
	}

	for(RTRResource* const resource : m_resourcesPendingCleanup)
	{
		auto removedResource = m_resources.remove(resource);
		if(removedResource)
		{
			removedResource->cleanupGHI(caller);
		}
		else
		{
			PH_LOG_WARNING(RTRScene,
				"on resource removal: did not find specified resource, nothing removed");
		}
	}
	m_resourcesPendingCleanup.clear();
}

void RTRScene::addResource(std::unique_ptr<RTRResource> resource)
{
	if(!resource)
	{
		PH_LOG_WARNING(RTRScene,
			"resource not added since it is empty");
		return;
	}

	RTRResource* const resourcePtr = resource.get();
	m_resources.add(std::move(resource));
	m_resourcesPendingSetup.push_back(resourcePtr);
}

void RTRScene::removeResource(RTRResource* const resource)
{
	if(!resource)
	{
		PH_LOG_WARNING(RTRScene,
			"resource not removed since it is empty");
		return;
	}

	m_resourcesPendingCleanup.push_back(resource);
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
	auto removedContent = m_customRenderContents.remove(content);
	if(!removedContent)
	{
		PH_LOG_WARNING(RTRScene,
			"on custom render content removal: did not find specified content, nothing removed");
	}
}

}// end namespace ph::editor
