#include "Render/RTRScene.h"
#include "Render/RenderThreadUpdateContext.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RTRScene, Render);

RTRScene::RTRScene() = default;

RTRScene::RTRScene(RTRScene&& other) = default;

RTRScene::~RTRScene()
{
	const auto numRemainingResources = 
		m_resources.size() +
		m_resourcesPendingAdd.size() +
		m_resourcesPendingRemove.size();
	if(numRemainingResources != 0)
	{
		PH_LOG_ERROR(RTRScene, 
			"{} resources are leaked; remove the resource when you are done with it", 
			numRemainingResources);
	}

	if(!m_customRenderContents.isEmpty())
	{
		PH_LOG_ERROR(RTRScene,
			"{} custom render contents are leaked; remove the content when you are done with it", 
			m_customRenderContents.size());
	}
}

void RTRScene::update(const RenderThreadUpdateContext& ctx)
{
	for(auto& customRenderContent : m_customRenderContents)
	{
		customRenderContent->update(ctx);
	}
}

void RTRScene::createGHICommands(GHIThreadCaller& caller)
{
	for(auto& resource : m_resourcesPendingAdd)
	{
		resource->setupGHI(caller);
	}
	m_resources.addAll(m_resourcesPendingAdd);

	// TODO: update dynamic resource

	for(auto& customRenderContent : m_customRenderContents)
	{
		customRenderContent->createGHICommands(caller);
	}

	for(auto& resource : m_resourcesPendingRemove)
	{
		resource->cleanupGHI(caller);
	}
	m_resourcesPendingRemove.removeAll();
}

void RTRScene::addResource(std::unique_ptr<RTRResource> resource)
{
	if(!resource)
	{
		PH_LOG_WARNING(RTRScene,
			"resource not added since it is empty");
		return;
	}

	m_resourcesPendingAdd.add(std::move(resource));
}

void RTRScene::removeResource(RTRResource* const resourcePtr)
{
	auto resource = m_resources.remove(resourcePtr);
	if(!resource)
	{
		PH_LOG_WARNING(RTRScene,
			"on resource removal: did not find specified resource, nothing removed");
		return;
	}
	
	m_resourcesPendingRemove.add(std::move(resource));
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

RTRScene& RTRScene::operator = (RTRScene&& rhs) = default;

}// end namespace ph::editor
