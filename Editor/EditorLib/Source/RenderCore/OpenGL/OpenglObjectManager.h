#pragma once

#include "RenderCore/GraphicsObjectManager.h"
#include "EditorCore/Storage/TTrivialItemPool.h"
#include "EditorCore/Storage/TConcurrentHandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "RenderCore/OpenGL/OpenglTexture.h"

#include <Utility/Concurrent/TAtomicQuasiQueue.h>
#include <Utility/TFunction.h>

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

struct OpenglObjectDeleter
{
	using DeleteOperation = TFunction<bool(), 32>;

	DeleteOperation op;

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

	void deleteTexture(GHITextureHandle handle) override;
	void deleteFramebuffer(GHIFramebufferHandle handle) override;
	void deleteShader(GHIShaderHandle handle) override;
	void deleteShaderProgram(GHIShaderProgramHandle handle) override;
	void deleteVertexStorage(GHIVertexStorageHandle handle) override;
	void deleteIndexStorage(GHIIndexStorageHandle handle) override;
	void deleteMesh(GHIMeshHandle handle) override;

	void beginFrameUpdate() override;
	void endFrameUpdate() override;

private:
	friend class OpenglGHI;

	OpenglTexture* getTexture(GHITextureHandle handle);

private:
	OpenglContext& m_ctx;

	template<typename OpenglObjType, CWeakHandle HandleType>
	using TPool = TTrivialItemPool<OpenglObjType, TConcurrentHandleDispatcher<HandleType>>;

	TPool<OpenglTexture, GHITextureHandle> m_textures;

	TAtomicQuasiQueue<OpenglObjectCreator> m_creationQueue;
	TAtomicQuasiQueue<OpenglObjectDeleter> m_deletionQueue;
	std::vector<OpenglObjectDeleter> m_failedDeleterCache;
};

inline OpenglTexture* OpenglObjectManager::getTexture(const GHITextureHandle handle)
{
	return m_textures.get(handle);
}

}// end namespace ph::editor
