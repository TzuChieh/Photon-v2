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
	if(!m_resources.isEmpty())
	{
		PH_LOG_ERROR(RTRScene, 
			"{} resources are leaked; remove the resource when you are done with it", 
			m_resources.size());
	}

	if(!m_customRenderContents.isEmpty())
	{
		PH_LOG_ERROR(RTRScene,
			"{} custom render contents are leaked; remove the content when you are done with it", 
			m_customRenderContents.size());
	}
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

void RTRScene::setupGHIForPendingResources(GHIThreadCaller& caller)
{
	for(RTRResource* const resourcePtr : m_resourcesPendingSetup)
	{
		resourcePtr->setupGHI(caller);
	}
	m_resourcesPendingSetup.clear();
}

void RTRScene::cleanupGHIForPendingResources(GHIThreadCaller& caller)
{
	for(RTRResource* const resourcePtr : m_resourcesPendingCleanup)
	{
		resourcePtr->cleanupGHI(caller);
	}

	// Add all cleaned-up resources for destroy before clearing the clean-up list
	m_resourcesPendingDestroy.insert(
		m_resourcesPendingDestroy.end(),
		m_resourcesPendingCleanup.begin(),
		m_resourcesPendingCleanup.end());

	m_resourcesPendingSetup.clear();
}

void RTRScene::destroyPendingResources()
{
	for(RTRResource* const resourcePtr : m_resourcesPendingDestroy)
	{
		auto resource = m_resources.remove(resourcePtr);
		if(!resource)
		{
			PH_LOG_WARNING(RTRScene,
				"on resource destruction: did not find specified resource, one resource not destroyed");
		}
	}
	m_resourcesPendingDestroy.clear();
}

void RTRScene::removeResource(RTRResource* const resourcePtr)
{
	if(!resourcePtr)
	{
		PH_LOG_WARNING(RTRScene,
			"resource removal ignored since it is empty");
		return;
	}
	
	m_resourcesPendingCleanup.push_back(resourcePtr);
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

void RTRScene::updateCustomRenderContents(const RenderThreadUpdateContext& ctx)
{
	for(auto& customRenderContent : m_customRenderContents)
	{
		customRenderContent->update(ctx);
	}
}

void RTRScene::createGHICommandsForCustomRenderContents(GHIThreadCaller& caller)
{
	for(auto& customRenderContent : m_customRenderContents)
	{
		customRenderContent->createGHICommands(caller);
	}
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
