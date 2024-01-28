#include "Render/Scene.h"
#include "Render/RenderThreadUpdateContext.h"
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

	, m_pendingSetups()
	, m_pendingCleanups()

	, m_resources()
	, m_resourcesPendingDestroy()
	, m_dynamicResources()
{}

Scene::~Scene()
{
	std::size_t numLeftoverContents = m_textures.numItems();
	if(numLeftoverContents != 0)
	{
		PH_LOG(Scene, Error,
			"{} render contents are still present on scene destruction; remove the contents when "
			"you are done with them.", numLeftoverContents);

		PH_LOG(Scene, Note,
			"Trying to cleanup leftover contents...");
		removeAllContents();
	}

	if(!m_pendingCleanups.empty())
	{
		PH_LOG(Scene, Error,
			"{} pending GHI cleanups are not executed", m_pendingCleanups.size());
	}

	if(!m_resources.isEmpty())
	{
		PH_LOG(Scene, Error,
			"{} resources are still present; manual resource removal before scene destruction is "
			"mandatory as we cannot foreseen any potential side effects such as dependency between "
			"resources.", m_resources.size());
	}
}

TextureHandle Scene::declareTexture()
{
	return m_textures.dispatchOneHandle();
}

void Scene::createTexture(TextureHandle handle, const ghi::TextureDesc& desc)
{
	m_textures.createAt(handle, {.desc = desc});

	Texture* texture = m_textures.get(handle);
	PH_ASSERT(texture);
	PH_ASSERT(!texture->handle);
	texture->handle = getSystem().getGraphicsContext().getObjectManager().createTexture(desc);
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
		PH_LOG(Scene, Error,
			"Cannot remove texture with invalid handle {}", handle);
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
	if(!texture || !texture->handle)
	{
		PH_LOG(Scene, Error,
			"Cannot load picture <{}>: texture={}, graphics handle={}",
			pictureFile, static_cast<void*>(texture), handle);
		return;
	}
	
	getSystem().addFileReadingWork(
		[&gCtx = getSystem().getGraphicsContext(), gHandle = texture->handle, pictureFile]()
		{
			RegularPicture picture;
			try
			{
				picture = io_utils::load_LDR_picture(pictureFile);
			}
			catch(const FileIOError& e)
			{
				PH_LOG(Scene, Error,
					"Cannot load picture: {}", e.whatStr());
			}

			auto pictureBytes = picture.getPixels().getBytes();
			ghi::GraphicsArena arena = gCtx.getMemoryManager().newRenderProducerHostArena();
			auto pixelData = arena.makeArray<std::byte>(pictureBytes.size());
			std::copy_n(pictureBytes.data(), pictureBytes.size(), pixelData.data());

			auto pixelFormat = ghi::EPixelFormat::RGB;
			switch(picture.numComponents())
			{
			case 1: pixelFormat = ghi::EPixelFormat::R; break;
			case 2: pixelFormat = ghi::EPixelFormat::RG; break;
			case 3: pixelFormat = ghi::EPixelFormat::RGB; break;
			case 4: pixelFormat = ghi::EPixelFormat::RGBA; break;
			default: PH_ASSERT_UNREACHABLE_SECTION(); break;
			}

			gCtx.getObjectManager().uploadPixelData(
				gHandle,
				pixelData,
				pixelFormat,
				ghi::translate_to<ghi::EPixelComponent>(picture.getComponentType()));
		});
}

void Scene::runPendingSetups(GHIThreadCaller& caller)
{
	for(auto& pendingSetup : m_pendingSetups)
	{
		pendingSetup(caller);
	}
	m_pendingSetups.clear();
}

void Scene::runPendingCleanups(GHIThreadCaller& caller)
{
	for(auto& pendingCleanup : m_pendingCleanups)
	{
		pendingCleanup(caller);
	}
	m_pendingCleanups.clear();
}

void Scene::addResource(std::unique_ptr<ISceneResource> resource)
{
	if(!resource)
	{
		return;
	}

	m_pendingSetups.push_back(
		[resource = resource.get()](GHIThreadCaller& caller)
		{
			resource->setupGHI(caller);
		});
	m_resources.add(std::move(resource));
}

void Scene::addDynamicResource(std::unique_ptr<IDynamicSceneResource> resource)
{
	if(!resource)
	{
		return;
	}

	m_pendingSetups.push_back(
		[this, resource = resource.get()](GHIThreadCaller& caller)
		{
			resource->setupGHI(caller);
			m_dynamicResources.addValue(resource);
			
		});
	m_resources.add(std::move(resource));
}

void Scene::removeResource(ISceneResource* resource)
{
	if(!resource)
	{
		return;
	}

	m_pendingCleanups.push_back(
		[this, resource](GHIThreadCaller& caller)
		{
			resource->cleanupGHI(caller);
			m_resourcesPendingDestroy.push_back(resource);
		});
}

void Scene::destroyRemovedResources()
{
	for(ISceneResource* resourcePtr : m_resourcesPendingDestroy)
	{
		auto resource = m_resources.remove(resourcePtr);
		if(!resource)
		{
			PH_LOG(Scene, Warning,
				"Did not find the specified resource, one resource not destroyed.");
		}
	}
	m_resourcesPendingDestroy.clear();
}

void Scene::updateDynamicResources(const RenderThreadUpdateContext& ctx)
{
	for(IDynamicSceneResource* resource : m_dynamicResources)
	{
		resource->update(ctx);
	}
}

void Scene::createGHICommandsForDynamicResources(GHIThreadCaller& caller)
{
	for(IDynamicSceneResource* resource : m_dynamicResources)
	{
		resource->createGHICommands(caller);
	}
}

void Scene::setSystem(System* sys)
{
	PH_ASSERT(sys);
	m_sys = sys;
}

void Scene::removeAllContents()
{
	PH_LOG(Scene, Note,
		"Removing all contents in {}.", getDebugName());

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
