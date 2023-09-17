#pragma once

#include "RenderCore/GraphicsObjectManager.h"
#include "EditorCore/Storage/TTrivialItemPool.h"
#include "EditorCore/Storage/TConcurrentHandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "RenderCore/OpenGL/OpenglTexture.h"

#include <Utility/Concurrent/TAtomicQuasiQueue.h>
#include <Utility/TFunction.h>
#include <Common/primitive_type.h>

#include <vector>

namespace ph::editor
{

class OpenglContext;

struct OpenglObjectCreator
{
	using CreateOperation = TFunction<void(), 64>;

	CreateOperation op;

	/*! @brief Run the create operation.
	*/
	inline void create() const
	{
		op();
	}
};

struct OpenglObjectManipulator
{
	using ManipulateOperation = TFunction<void(), 64>;

	ManipulateOperation op;

	/*! @brief Run the manipulate operation.
	*/
	inline void manipulate() const
	{
		op();
	}
};

struct OpenglObjectDeleter
{
	using DeleteOperation = TFunction<bool(), 32>;

	DeleteOperation op;
	uint32 numRetries = 0;

	/*! @brief Run the delete operation.
	@return True if the operation is done and the target object has been cleaned up. False if the
	operation cannot run now and should be retried in a later time.
	*/
	inline bool tryDelete() const
	{
		return op();
	}
};

class OpenglObjectManager final : public GraphicsObjectManager
{
public:
	explicit OpenglObjectManager(OpenglContext& ctx);
	~OpenglObjectManager() override;

	GHITextureHandle createTexture(const GHIInfoTextureDesc& desc) override;

	GHIFramebufferHandle createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) override;

	GHIShaderHandle createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) override;

	GHIShaderProgramHandle createShaderProgram(
		std::string name,
		const GHIInfoShaderSet& shaders) override;

	GHIVertexStorageHandle createVertexStorage(
		std::size_t numVertices,
		const GHIInfoVertexGroupFormat& format,
		EGHIStorageUsage usage) override;

	GHIIndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EGHIStorageElement indexType,
		EGHIStorageUsage usage) override;

	GHIMeshHandle createMesh(
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		const GHIInfoMeshVertexLayout& layout,
		GHIIndexStorageHandle indexStorage) override;

	void uploadPixelData(
		GHITextureHandle handle,
		TSpanView<std::byte> pixelData,
		EGHIPixelFormat pixelFormat,
		EGHIPixelComponent pixelComponent) override;

	void removeTexture(GHITextureHandle handle) override;
	void removeFramebuffer(GHIFramebufferHandle handle) override;
	void removeShader(GHIShaderHandle handle) override;
	void removeShaderProgram(GHIShaderProgramHandle handle) override;
	void removeVertexStorage(GHIVertexStorageHandle handle) override;
	void removeIndexStorage(GHIIndexStorageHandle handle) override;
	void removeMesh(GHIMeshHandle handle) override;

	void onGHILoad() override;
	void onGHIUnload() override;
	void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override;
	void endFrameUpdate(const GHIThreadUpdateContext& ctx) override;

private:
	friend class OpenglGHI;

	OpenglTexture* getTexture(GHITextureHandle handle);

private:
	void deleteAllObjects();
	void deleteAllTextures();

	OpenglContext& m_ctx;

	template<typename OpenglObjType, CWeakHandle HandleType>
	using TPool = TTrivialItemPool<OpenglObjType, TConcurrentHandleDispatcher<HandleType>>;

	TPool<OpenglTexture, GHITextureHandle> m_textures;

	TAtomicQuasiQueue<OpenglObjectCreator> m_creationQueue;
	TAtomicQuasiQueue<OpenglObjectManipulator> m_manipulationQueue;
	TAtomicQuasiQueue<OpenglObjectDeleter> m_deletionQueue;
	std::vector<OpenglObjectDeleter> m_failedDeleterCache;
};

inline OpenglTexture* OpenglObjectManager::getTexture(const GHITextureHandle handle)
{
	return m_textures.get(handle);
}

}// end namespace ph::editor
