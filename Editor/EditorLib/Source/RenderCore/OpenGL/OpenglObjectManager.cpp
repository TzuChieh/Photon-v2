#include "RenderCore/OpenGL/OpenglObjectManager.h"
#include "RenderCore/OpenGL/OpenglContext.h"

#include <Common/logging.h>

#include <cstddef>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(OpenglObjectManager, GHI);

OpenglObjectManager::OpenglObjectManager(OpenglContext& ctx)

	: GraphicsObjectManager()

	, m_ctx(ctx)

	, m_textures()

	, m_creationQueue()
	, m_deletionQueue()
	, m_failedDeleterCache()
{}

OpenglObjectManager::~OpenglObjectManager() = default;

GHITextureHandle OpenglObjectManager::createTexture(const GHIInfoTextureDesc& desc)
{
	GHITextureHandle handle = m_textures.dispatchOneHandle();

	OpenglObjectCreator creator;
	creator.op = [&textures = m_textures, desc, handle]()
	{
		OpenglTexture texture;
		texture.createImmutableStorage(desc);
		textures.createAt(handle, texture);
	};
	m_creationQueue.enqueue(creator);

	return handle;
}

GHIFramebufferHandle OpenglObjectManager::createFramebuffer(
	const GHIInfoFramebufferAttachment& attachments)
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
	EGHIPixelComponent componentType)
{
	OpenglObjectCreator creator;
	creator.op = [&textures = m_textures, pixelData, handle, componentType]()
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

		texture->uploadPixelData(pixelData, componentType);
	};
	m_creationQueue.enqueue(creator);
}

void OpenglObjectManager::removeTexture(const GHITextureHandle handle)
{
	if(!handle)
	{
		return;
	}

	OpenglObjectDeleter deleter;
	deleter.op = [&textures = m_textures, handle]() -> bool
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
	for(std::size_t i = 0; i < m_textures.capacity(); ++i)
	{
		if(m_textures[i].hasResource())
		{
			m_textures[i].destroy();
			++numDeleted;
		}
	}

	PH_LOG(OpenglObjectManager,
		"Deleted {} textures. Storage capacity = {}.",
		numDeleted, m_textures.capacity());
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
	OpenglObjectCreator creator;
	while(m_creationQueue.tryDequeue(&creator))
	{
		creator.create();
	}
}

void OpenglObjectManager::endFrameUpdate(const GHIThreadUpdateContext& ctx)
{
	OpenglObjectDeleter deleter;
	while(m_deletionQueue.tryDequeue(&deleter))
	{
		// Failed attempts need to be retried later
		if(!deleter.tryDelete())
		{
			m_failedDeleterCache.push_back(deleter);
		}
	}

	// Retry failed attempts on next frame while preserving their original order
	m_deletionQueue.enqueueBulk(m_failedDeleterCache.begin(), m_failedDeleterCache.size());
	m_failedDeleterCache.clear();
}

}// end namespace ph::editor
