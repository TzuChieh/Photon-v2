#include "Render/RendererScene.h"
#include "Render/RenderThreadUpdateContext.h"
#include "Render/System.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/ghi_enums.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <DataIO/io_utils.h>
#include <DataIO/FileSystem/Path.h>
#include <Frame/RegularPicture.h>

#include <utility>
#include <algorithm>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(RendererScene, Render);

RendererScene::RendererScene(render::System& sys)
	: mainView()

	, m_sys(sys)

	, m_textures()

	, m_resources()
	, m_resourcesPendingSetup()
	, m_resourcesPendingCleanup()
	, m_resourcesPendingDestroy()
	, m_customRenderContents()
{}

RendererScene::~RendererScene()
{
	bool hasRemainedResources = false;

	if(!m_resources.isEmpty())
	{
		hasRemainedResources = true;

		PH_LOG_ERROR(RendererScene,
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
		PH_LOG_WARNING(RendererScene,
			"remained resources detected on scene destruction");

		reportResourceStates();
	}
}

render::TextureHandle RendererScene::declareTexture()
{
	return m_textures.dispatchOneHandle();
}

void RendererScene::createTexture(render::TextureHandle handle, render::Texture texture)
{
	m_textures.createAt(handle, std::move(texture));
}

void RendererScene::removeTexture(render::TextureHandle handle)
{
	m_textures.remove(handle);
}

void RendererScene::loadPicture(render::TextureHandle handle, const Path& pictureFile)
{
	render::Texture* texture = m_textures.get(handle);
	if(!texture)
	{
		PH_LOG_WARNING(RendererScene,
			"Cannot load picture <{}> with invalid handle ({})",
			pictureFile, handle.toString());
		return;
	}
	
	if(!texture->handle)
	{
		texture->handle = m_sys.getGraphicsContext().getObjectManager().createTexture(texture->desc);
	}

	m_sys.addFileReadingWork([
		&gCtx = m_sys.getGraphicsContext(),
		gHandle = texture->handle,
		pictureFile]()
		{
			RegularPicture picture = io_utils::load_LDR_picture(pictureFile);
			auto pictureBytes = picture.getPixels().getBytes();

			GraphicsArena arena = gCtx.getMemoryManager().getRendererHostArena();
			auto gBytes = arena.makeArray<std::byte>(pictureBytes.size());
			std::copy_n(pictureBytes.data(), pictureBytes.size(), gBytes.data());

			gCtx.getObjectManager().uploadPixelData(
				gHandle,
				gBytes,
				translate_to<EGHIPixelComponent>(picture.getComponentType()));
		});
}

void RendererScene::addResource(std::unique_ptr<RendererResource> resource)
{
	if(!resource)
	{
		PH_LOG_WARNING(RendererScene,
			"resource not added since it is empty");
		return;
	}

	RendererResource* const resourcePtr = resource.get();
	m_resources.add(std::move(resource));
	m_resourcesPendingSetup.push_back(resourcePtr);
}

void RendererScene::setupGHIForPendingResources(GHIThreadCaller& caller)
{
	for(RendererResource* const resource : m_resourcesPendingSetup)
	{
		resource->setupGHI(caller);
	}
	m_resourcesPendingSetup.clear();
}

void RendererScene::cleanupGHIForPendingResources(GHIThreadCaller& caller)
{
	for(RendererResource* const resource : m_resourcesPendingCleanup)
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

void RendererScene::destroyPendingResources()
{
	for(RendererResource* const resourcePtr : m_resourcesPendingDestroy)
	{
		auto resource = m_resources.remove(resourcePtr);
		if(!resource)
		{
			PH_LOG_WARNING(RendererScene,
				"on resource destruction: did not find specified resource, one resource not destroyed");
		}
	}
	m_resourcesPendingDestroy.clear();
}

void RendererScene::removeResource(RendererResource* const resourcePtr)
{
	if(!resourcePtr)
	{
		PH_LOG_WARNING(RendererScene,
			"resource removal ignored since it is empty");
		return;
	}
	
	m_resourcesPendingCleanup.push_back(resourcePtr);
}

void RendererScene::addCustomRenderContent(std::unique_ptr<CustomRenderContent> content)
{
	if(!content)
	{
		PH_LOG_WARNING(RendererScene,
			"custom render content ignored since it is empty");
		return;
	}

	m_customRenderContents.push_back(content.get());
	addResource(std::move(content));
}

void RendererScene::updateCustomRenderContents(const RenderThreadUpdateContext& ctx)
{
	for(CustomRenderContent* const content : m_customRenderContents)
	{
		content->update(ctx);
	}
}

void RendererScene::createGHICommandsForCustomRenderContents(GHIThreadCaller& caller)
{
	for(CustomRenderContent* const content : m_customRenderContents)
	{
		content->createGHICommands(caller);
	}
}

void RendererScene::removeCustomRenderContent(CustomRenderContent* const content)
{
	for(CustomRenderContent*& content : m_customRenderContents)
	{
		removeResource(content);
		content = nullptr;
	}

	const auto numErasedContents = std::erase(m_customRenderContents, nullptr);
	if(numErasedContents != 1)
	{
		PH_LOG_WARNING(RendererScene,
			"on custom render content removal: {}",
			numErasedContents == 0 ? "content not found" : "duplicates found and removed");
	}
}

void RendererScene::reportResourceStates()
{
	PH_LOG(RendererScene,
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

}// end namespace ph::editor
