#pragma once

#include "RenderCore/GraphicsObjectManager.h"
#include "EditorCore/Storage/TTrivialItemPool.h"
#include "EditorCore/Storage/TConcurrentHandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "RenderCore/OpenGL/OpenglTexture.h"
#include "RenderCore/OpenGL/OpenglFramebuffer.h"

#include <Utility/Concurrent/TAtomicQuasiQueue.h>
#include <Utility/TFunction.h>
#include <Common/primitive_type.h>

#include <vector>

namespace ph::editor::ghi
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

	TextureHandle createTexture(const TextureDesc& desc) override;

	FramebufferHandle createFramebuffer(
		const FramebufferDesc& desc) override;

	ShaderHandle createShader(
		std::string name,
		EShadingStage shadingStage,
		std::string shaderSource) override;

	ShaderProgramHandle createShaderProgram(
		std::string name,
		const ShaderSetInfo& shaders) override;

	VertexStorageHandle createVertexStorage(
		std::size_t numVertices,
		const VertexGroupFormatInfo& format,
		EStorageUsage usage) override;

	IndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EStorageElement indexType,
		EStorageUsage usage) override;

	MeshHandle createMesh(
		TSpanView<VertexStorageHandle> vertexStorages,
		const MeshVertexLayoutInfo& layout,
		IndexStorageHandle indexStorage) override;

	void uploadPixelData(
		TextureHandle handle,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) override;

	void removeTexture(TextureHandle handle) override;
	void removeFramebuffer(FramebufferHandle handle) override;
	void removeShader(ShaderHandle handle) override;
	void removeShaderProgram(ShaderProgramHandle handle) override;
	void removeVertexStorage(VertexStorageHandle handle) override;
	void removeIndexStorage(IndexStorageHandle handle) override;
	void removeMesh(MeshHandle handle) override;

	void onGHILoad() override;
	void onGHIUnload() override;
	void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override;
	void endFrameUpdate(const GHIThreadUpdateContext& ctx) override;

public:
	template<typename OpenglObjType, CWeakHandle HandleType>
	using TPool = TTrivialItemPool<OpenglObjType, TConcurrentHandleDispatcher<HandleType>>;

	TPool<OpenglTexture, TextureHandle> textures;
	TPool<OpenglFramebuffer, FramebufferHandle> framebuffers;

private:
	void deleteAllObjects();
	void deleteAllTextures();

	OpenglContext& m_ctx;
	TAtomicQuasiQueue<OpenglObjectCreator> m_creationQueue;
	TAtomicQuasiQueue<OpenglObjectManipulator> m_manipulationQueue;
	TAtomicQuasiQueue<OpenglObjectDeleter> m_deletionQueue;
	std::vector<OpenglObjectDeleter> m_failedDeleterCache;
};

}// end namespace ph::editor::ghi
