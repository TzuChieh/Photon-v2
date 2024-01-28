#include "RenderCore/OpenGL/OpenglObjectManager.h"
#include "RenderCore/OpenGL/OpenglContext.h"

#include <Common/logging.h>
#include <Common/profiling.h>

#include <cstddef>

namespace ph::editor::ghi
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

TextureHandle OpenglObjectManager::createTexture(const TextureDesc& desc)
{
	PH_PROFILE_SCOPE();

	TextureHandle handle = textures.dispatchOneHandle();

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

FramebufferHandle OpenglObjectManager::createFramebuffer(
	const FramebufferDesc& desc)
{
	return FramebufferHandle{};
}

ShaderHandle OpenglObjectManager::createShader(
	std::string name,
	EShadingStage shadingStage,
	std::string shaderSource)
{
	return ShaderHandle{};
}

ShaderProgramHandle OpenglObjectManager::createShaderProgram(
	std::string name,
	const ShaderSetInfo& shaders)
{
	return ShaderProgramHandle{};
}

VertexStorageHandle OpenglObjectManager::createVertexStorage(
	std::size_t numVertices,
	const VertexGroupFormatInfo& format,
	EStorageUsage usage)
{
	return VertexStorageHandle{};
}

IndexStorageHandle OpenglObjectManager::createIndexStorage(
	std::size_t numIndices,
	EStorageElement indexType,
	EStorageUsage usage)
{
	return IndexStorageHandle{};
}

MeshHandle OpenglObjectManager::createMesh(
	TSpanView<VertexStorageHandle> vertexStorages,
	const MeshVertexLayoutInfo& layout,
	IndexStorageHandle indexStorage)
{
	return MeshHandle{};
}

void OpenglObjectManager::uploadPixelData(
	TextureHandle handle,
	TSpanView<std::byte> pixelData,
	EPixelFormat pixelFormat,
	EPixelComponent pixelComponent)
{
	PH_PROFILE_SCOPE();

	OpenglObjectManipulator manipulator;
	manipulator.op = [&textures = textures, pixelData, handle, pixelFormat, pixelComponent]()
	{
		OpenglTexture* texture = textures.get(handle);
		if(!texture || !texture->hasResource())
		{
			PH_LOG(OpenglObjectManager, Error,
				"Cannot upload pixel data for texture; object: {}, resource: {}, handle: {}",
				static_cast<void*>(texture), texture ? texture->hasResource() : false, handle);
			return;
		}

		texture->uploadPixelData(pixelData, pixelFormat, pixelComponent);
	};
	m_manipulationQueue.enqueue(manipulator);
}

void OpenglObjectManager::removeTexture(const TextureHandle handle)
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

void OpenglObjectManager::removeFramebuffer(FramebufferHandle handle)
{}

void OpenglObjectManager::removeShader(ShaderHandle handle)
{}

void OpenglObjectManager::removeShaderProgram(ShaderProgramHandle handle)
{}

void OpenglObjectManager::removeVertexStorage(VertexStorageHandle handle)
{}

void OpenglObjectManager::removeIndexStorage(IndexStorageHandle handle)
{}

void OpenglObjectManager::removeMesh(MeshHandle handle)
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

	PH_LOG(OpenglObjectManager, Note,
		"Deleted {} textures. Storage capacity = {}.",
		numDeleted, textures.capacity());
}

void OpenglObjectManager::onGHILoad()
{}

void OpenglObjectManager::onGHIUnload()
{
	PH_LOG(OpenglObjectManager, Note,
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

	PH_LOG(OpenglObjectManager, Note,
		"performed {} pending deletes, {} were successful.",
		numAttempts, numAttempts - numFailedAttempts);
	if(numFailedAttempts != 0)
	{
		PH_LOG(OpenglObjectManager, Error,
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
				PH_LOG(OpenglObjectManager, Warning,
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

}// end namespace ph::editor::ghi
