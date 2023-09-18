#include "Render/Scene.h"
#include "Render/UpdateContext.h"
#include "Render/System.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/ghi_enums.h"

#include <Common/logging.h>
#include <DataIO/io_utils.h>
#include <DataIO/FileSystem/Path.h>
#include <Frame/RegularPicture.h>
#include <DataIO/io_exceptions.h>

#include <utility>
#include <algorithm>

namespace ph::editor::render
{

PH_DEFINE_INTERNAL_LOG_GROUP(Scene, Render);

Scene::Scene()
	: Scene("")
{}

Scene::Scene(std::string debugName)
	: mainView()

	, m_sys(nullptr)
	, m_debugName(std::move(debugName))

	, m_textures()

	, m_resources()
	, m_resourcesPendingSetup()
	, m_resourcesPendingCleanup()
	, m_resourcesPendingDestroy()
	, m_customRenderContents()
{}

Scene::~Scene()
{
	std::size_t numLeftoverContents = m_textures.numItems();
	if(numLeftoverContents != 0)
	{
		PH_LOG_ERROR(Scene,
			"{} render contents are still present on scene destruction; remove the contents when "
			"you are done with them.", numLeftoverContents);

		PH_LOG(Scene,
			"Trying to cleanup leftover contents...");
		removeAllContents();
	}

	bool hasLeakedResources = false;

	if(!m_resources.isEmpty())
	{
		PH_LOG_ERROR(Scene,
			"{} resources are leaked; remove the resource when you are done with it", 
			m_resources.size());
		hasLeakedResources = true;
	}

	if(!m_resourcesPendingSetup.empty() ||
       !m_resourcesPendingCleanup.empty() ||
       !m_resourcesPendingDestroy.empty())
	{
		hasLeakedResources = true;
	}

	if(hasLeakedResources)
	{
		PH_LOG_WARNING(Scene,
			"remained resources detected on scene (name: {}) destruction",
			m_debugName);
		reportResourceStates();
	}
}

TextureHandle Scene::declareTexture()
{
	return m_textures.dispatchOneHandle();
}

void Scene::createTexture(TextureHandle handle, Texture texture)
{
	m_textures.createAt(handle, std::move(texture));
}

Texture* Scene::getTexture(TextureHandle handle)
{
	return m_textures.get(handle);
}

void Scene::removeTexture(TextureHandle handle)
{
	Texture* texture = m_textures.get(handle);
	if(!texture)
	{
		PH_LOG_ERROR(Scene,
			"Cannot remove texture with invalid handle ({})",
			handle.toString());
		return;
	}

	if(texture->handle)
	{
		getSystem().getGraphicsContext().getObjectManager().removeTexture(texture->handle);
	}

	m_textures.remove(handle);
}

void Scene::loadPicture(TextureHandle handle, const Path& pictureFile)
{
	Texture* texture = m_textures.get(handle);
	if(!texture)
	{
		PH_LOG_ERROR(Scene,
			"Cannot load picture <{}> with invalid handle ({})",
			pictureFile, handle.toString());
		return;
	}
	
	GraphicsContext& gCtx = getSystem().getGraphicsContext();
	if(!texture->handle)
	{
		texture->handle = gCtx.getObjectManager().createTexture(texture->desc);
	}

	getSystem().addFileReadingWork(
		[&gCtx, gHandle = texture->handle, pictureFile]()
		{
			RegularPicture picture;
			try
			{
				picture = io_utils::load_LDR_picture(pictureFile);
			}
			catch(const FileIOError& e)
			{
				PH_LOG_ERROR(Scene,
					"Cannot load picture: {}", e.whatStr());
			}

			auto pictureBytes = picture.getPixels().getBytes();
			GraphicsArena arena = gCtx.getMemoryManager().newRenderProducerHostArena();
			auto pixelData = arena.makeArray<std::byte>(pictureBytes.size());
			std::copy_n(pictureBytes.data(), pictureBytes.size(), pixelData.data());

			auto pixelFormat = EGHIPixelFormat::RGB;
			switch(picture.numComponents())
			{
			case 1: pixelFormat = EGHIPixelFormat::R; break;
			case 2: pixelFormat = EGHIPixelFormat::RG; break;
			case 3: pixelFormat = EGHIPixelFormat::RGB; break;
			case 4: pixelFormat = EGHIPixelFormat::RGBA; break;
			default: PH_ASSERT_UNREACHABLE_SECTION(); break;
			}

			gCtx.getObjectManager().uploadPixelData(
				gHandle,
				pixelData,
				pixelFormat,
				translate_to<EGHIPixelComponent>(picture.getComponentType()));
		});
}

void Scene::addResource(std::unique_ptr<SceneResource> resource)
{
	if(!resource)
	{
		PH_LOG_WARNING(Scene,
			"resource not added since it is empty");
		return;
	}

	SceneResource* const resourcePtr = resource.get();
	m_resources.add(std::move(resource));
	m_resourcesPendingSetup.push_back(resourcePtr);
}

void Scene::setupGHIForPendingResources(GHIThreadCaller& caller)
{
	for(SceneResource* const resource : m_resourcesPendingSetup)
	{
		resource->setupGHI(caller);
	}
	m_resourcesPendingSetup.clear();
}

void Scene::cleanupGHIForPendingResources(GHIThreadCaller& caller)
{
	for(SceneResource* const resource : m_resourcesPendingCleanup)
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

void Scene::destroyPendingResources()
{
	for(SceneResource* const resourcePtr : m_resourcesPendingDestroy)
	{
		auto resource = m_resources.remove(resourcePtr);
		if(!resource)
		{
			PH_LOG_WARNING(Scene,
				"on resource destruction: did not find specified resource, one resource not destroyed");
		}
	}
	m_resourcesPendingDestroy.clear();
}

void Scene::removeResource(SceneResource* const resourcePtr)
{
	if(!resourcePtr)
	{
		PH_LOG_WARNING(Scene,
			"resource removal ignored since it is empty");
		return;
	}
	
	m_resourcesPendingCleanup.push_back(resourcePtr);
}

void Scene::addCustomRenderContent(std::unique_ptr<CustomContent> content)
{
	if(!content)
	{
		PH_LOG_WARNING(Scene,
			"custom render content ignored since it is empty");
		return;
	}

	m_customRenderContents.push_back(content.get());
	addResource(std::move(content));
}

void Scene::updateCustomRenderContents(const UpdateContext& ctx)
{
	for(CustomContent* const content : m_customRenderContents)
	{
		content->update(ctx);
	}
}

void Scene::createGHICommandsForCustomRenderContents(GHIThreadCaller& caller)
{
	for(CustomContent* const content : m_customRenderContents)
	{
		content->createGHICommands(caller);
	}
}

void Scene::removeCustomRenderContent(CustomContent* const content)
{
	for(CustomContent*& content : m_customRenderContents)
	{
		removeResource(content);
		content = nullptr;
	}

	const auto numErasedContents = std::erase(m_customRenderContents, nullptr);
	if(numErasedContents != 1)
	{
		PH_LOG_WARNING(Scene,
			"on custom render content removal: {}",
			numErasedContents == 0 ? "content not found" : "duplicates found and removed");
	}
}

void Scene::reportResourceStates()
{
	PH_LOG(Scene,
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

void Scene::setSystem(System* sys)
{
	PH_ASSERT(sys);
	m_sys = sys;
}

void Scene::removeAllContents()
{
	// Remove textures
	{
		auto textureIter = m_textures.begin();
		while(textureIter != m_textures.end())
		{
			removeTexture(textureIter.getHandle());
			++textureIter;
		}
	}
}

}// end namespace ph::editor::render
