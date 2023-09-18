#include "RenderCore/OpenGL/OpenglObjectManager.h"
#include "RenderCore/OpenGL/OpenglContext.h"

#include <Common/logging.h>
#include <Common/profiling.h>

#include <cstddef>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(OpenglObjectManager, GHI);

OpenglObjectManager::OpenglObjectManager(OpenglContext& ctx)

	: GraphicsObjectManager()

	, textures()
	, framebuffers()

	, m_ctx(ctx)
	, m_creationQueue()
	, m_manipulationQueue()
	, m_deletionQueue()
	, m_failedDeleterCache()
{}

OpenglObjectManager::~OpenglObjectManager() = default;

GHITextureHandle OpenglObjectManager::createTexture(const GHIInfoTextureDesc& desc)
{
	PH_PROFILE_SCOPE();

	GHITextureHandle handle = textures.dispatchOneHandle();

	OpenglObjectCreator creator;
	creator.op = [&textures = textures, desc, handle]()
	{
		OpenglTexture texture;
		texture.create(desc);
		textures.createAt(handle, texture);
	};
	m_creationQueue.enqueue(creator);

	return handle;
}

GHIFramebufferHandle OpenglObjectManager::createFramebuffer(
	const GHIInfoFramebufferDesc& desc)
{
	return GHIFramebufferHandle{};
}

GHIShaderHandle OpenglObjectManager::createShader(
	std::string name,
	EGHIShadingStage shadingStage,
	std::string shaderSource)
{
	return GHIShaderHandle{};
}

GHIShaderProgramHandle OpenglObjectManager::createShaderProgram(
	std::string name,
	const GHIInfoShaderSet& shaders)
{
	return GHIShaderProgramHandle{};
}

GHIVertexStorageHandle OpenglObjectManager::createVertexStorage(
	std::size_t numVertices,
	const GHIInfoVertexGroupFormat& format,
	EGHIStorageUsage usage)
{
	return GHIVertexStorageHandle{};
}

GHIIndexStorageHandle OpenglObjectManager::createIndexStorage(
	std::size_t numIndices,
	EGHIStorageElement indexType,
	EGHIStorageUsage usage)
{
	return GHIIndexStorageHandle{};
}

GHIMeshHandle OpenglObjectManager::createMesh(
	TSpanView<GHIVertexStorageHandle> vertexStorages,
	const GHIInfoMeshVertexLayout& layout,
	GHIIndexStorageHandle indexStorage)
{
	return GHIMeshHandle{};
}

void OpenglObjectManager::uploadPixelData(
	GHITextureHandle handle,
	TSpanView<std::byte> pixelData,
	EGHIPixelFormat pixelFormat,
	EGHIPixelComponent pixelComponent)
{
	PH_PROFILE_SCOPE();

	OpenglObjectManipulator manipulator;
	manipulator.op = [&textures = textures, pixelData, handle, pixelFormat, pixelComponent]()
	{
		OpenglTexture* texture = textures.get(handle);
		if(!texture || !texture->hasResource())
		{
			PH_LOG_ERROR(OpenglObjectManager,
				"Cannot upload pixel data for texture; object: {}, resource: {}, handle: <{}>",
				static_cast<void*>(texture),
				texture ? texture->hasResource() : false,
				handle.toString());
			return;
		}

		texture->uploadPixelData(pixelData, pixelFormat, pixelComponent);
	};
	m_manipulationQueue.enqueue(manipulator);
}

void OpenglObjectManager::removeTexture(const GHITextureHandle handle)
{
	PH_PROFILE_SCOPE();

	if(!handle)
	{
		return;
	}

	OpenglObjectDeleter deleter;
	deleter.op = [&textures = textures, handle]() -> bool
	{
		OpenglTexture* texture = textures.get(handle);
		if(!texture || !texture->hasResource())
		{
			return false;
		}

		texture->destroy();
		textures.remove(handle);
		return true;
	};
	m_deletionQueue.enqueue(deleter);
}

void OpenglObjectManager::removeFramebuffer(GHIFramebufferHandle handle)
{}

void OpenglObjectManager::removeShader(GHIShaderHandle handle)
{}

void OpenglObjectManager::removeShaderProgram(GHIShaderProgramHandle handle)
{}

void OpenglObjectManager::removeVertexStorage(GHIVertexStorageHandle handle)
{}

void OpenglObjectManager::removeIndexStorage(GHIIndexStorageHandle handle)
{}

void OpenglObjectManager::removeMesh(GHIMeshHandle handle)
{}

void OpenglObjectManager::deleteAllObjects()
{
	deleteAllTextures();
}

void OpenglObjectManager::deleteAllTextures()
{
	std::size_t numDeleted = 0;
	for(std::size_t i = 0; i < textures.capacity(); ++i)
	{
		if(textures[i].hasResource())
		{
			textures[i].destroy();
			++numDeleted;
		}
	}

	PH_LOG(OpenglObjectManager,
		"Deleted {} textures. Storage capacity = {}.",
		numDeleted, textures.capacity());
}

void OpenglObjectManager::onGHILoad()
{}

void OpenglObjectManager::onGHIUnload()
{
	PH_LOG(OpenglObjectManager,
		"Start cleaning up resource objects...");

	// Perform pending deletes
	OpenglObjectDeleter deleter;
	std::size_t numAttempts = 0;
	std::size_t numFailedAttempts = 0;
	while(m_deletionQueue.tryDequeue(&deleter))
	{
		++numAttempts;
		if(!deleter.tryDelete())
		{
			++numFailedAttempts;
		}
	}

	PH_LOG(OpenglObjectManager,
		"performed {} pending deletes, {} were successful.",
		numAttempts, numAttempts - numFailedAttempts);
	if(numFailedAttempts != 0)
	{
		PH_LOG_ERROR(OpenglObjectManager,
			"{} out of {} deletes were unsuccessful. Please ensure correct object lifecycle.",
			numFailedAttempts, numAttempts);
	}

	deleteAllObjects();
}

void OpenglObjectManager::beginFrameUpdate(const GHIThreadUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	OpenglObjectCreator creator;
	while(m_creationQueue.tryDequeue(&creator))
	{
		creator.create();
	}

	// Manipulate objects between creation & deletion
	OpenglObjectManipulator manipulator;
	while(m_manipulationQueue.tryDequeue(&manipulator))
	{
		manipulator.manipulate();
	}
}

void OpenglObjectManager::endFrameUpdate(const GHIThreadUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	OpenglObjectDeleter deleter;
	while(m_deletionQueue.tryDequeue(&deleter))
	{
		// Failed attempts need to be retried later
		if(!deleter.tryDelete())
		{
			// A limit that will be reached in 1 minute, assuming 60 FPS and one try per frame
			constexpr uint32 maxRetries = 60 * 60;

			++deleter.numRetries;
			if(deleter.numRetries >= maxRetries)
			{
				PH_LOG_WARNING(OpenglObjectManager,
					"Detected hanging object deleter ({} retries), canceling",
					deleter.numRetries);
				continue;
			}
			
			m_failedDeleterCache.push_back(deleter);
		}
	}

	// Retry failed attempts on next frame while preserving their original order
	m_deletionQueue.enqueueBulk(m_failedDeleterCache.begin(), m_failedDeleterCache.size());
	m_failedDeleterCache.clear();
}

}// end namespace ph::editor
