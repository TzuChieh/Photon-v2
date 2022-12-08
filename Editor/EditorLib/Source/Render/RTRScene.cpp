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
	bool hasRemainedResources = false;

	if(!m_resources.isEmpty())
	{
		hasRemainedResources = true;

		PH_LOG_ERROR(RTRScene, 
			"{} resources are leaked; remove the resource when you are done with it", 
			m_resources.size());
	}

	if(!m_resourcesPendingSetup.empty() ||
       !m_resourcesPendingCleanup.empty() ||
       !m_resourcesPendingDestroy.empty())
	{
		hasRemainedResources = true;
	}

	if(hasRemainedResources)
	{
		PH_LOG_WARNING(RTRScene,
			"remained resources detected on scene destruction");

		reportResourceStates();
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
	for(RTRResource* const resource : m_resourcesPendingSetup)
	{
		resource->setupGHI(caller);
	}
	m_resourcesPendingSetup.clear();
}

void RTRScene::cleanupGHIForPendingResources(GHIThreadCaller& caller)
{
	for(RTRResource* const resource : m_resourcesPendingCleanup)
	{
		resource->cleanupGHI(caller);
	}

	// Add all cleaned-up resources for destroy before clearing the clean-up list
	m_resourcesPendingDestroy.insert(
		m_resourcesPendingDestroy.end(),
		m_resourcesPendingCleanup.begin(),
		m_resourcesPendingCleanup.end());

	m_resourcesPendingCleanup.clear();
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

	m_customRenderContents.push_back(content.get());
	addResource(std::move(content));
}

void RTRScene::updateCustomRenderContents(const RenderThreadUpdateContext& ctx)
{
	for(CustomRenderContent* const content : m_customRenderContents)
	{
		content->update(ctx);
	}
}

void RTRScene::createGHICommandsForCustomRenderContents(GHIThreadCaller& caller)
{
	for(CustomRenderContent* const content : m_customRenderContents)
	{
		content->createGHICommands(caller);
	}
}

void RTRScene::removeCustomRenderContent(CustomRenderContent* const content)
{
	for(CustomRenderContent*& content : m_customRenderContents)
	{
		removeResource(content);
		content = nullptr;
	}

	const auto numErasedContents = std::erase(m_customRenderContents, nullptr);
	if(numErasedContents != 1)
	{
		PH_LOG_WARNING(RTRScene,
			"on custom render content removal: {}",
			numErasedContents == 0 ? "content not found" : "duplicates found and removed");
	}
}

void RTRScene::reportResourceStates()
{
	PH_LOG(RTRScene,
		"stats on resources:\n"
		"# resources: {}\n"
		"# pending setup: {}\n"
		"# pending cleanup: {}\n"
		"# pending destroy: {}",
		m_resources.size(),
		m_resourcesPendingSetup.size(),
		m_resourcesPendingCleanup.size(),
		m_resourcesPendingDestroy.size());
}

RTRScene& RTRScene::operator = (RTRScene&& rhs) = default;

}// end namespace ph::editor
