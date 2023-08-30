#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>
#include <Utility/TSpan.h>
#include <Utility/TFunction.h>
#include <Utility/Concurrent/TAtomicQueue.h>

#include <cstddef>
#include <string>

namespace ph::editor
{

class GHIThread;

struct GraphicsObjectCreater
{
	using CreateOperation = TFunction<void(), 32>;

	CreateOperation op;

	/*! @brief Run the create operation.
	*/
	inline void create() const
	{
		op();
	}
};

struct GraphicsObjectDeleter
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

/*! @brief Manages the creation and deletion of graphics-related resource objects.
All `create<XXX>()` and `delete<XXX>()` methods are thread safe, as long as they are called within
the lifetime of current graphics context.
*/
class GraphicsObjectManager
{
public:
	virtual ~GraphicsObjectManager();

	/*!
	@note Thread safe.
	*/
	virtual GHITextureHandle createTexture(
		const GHIInfoTextureFormat& format,
		const math::Vector3UI& sizePx) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIFramebufferHandle createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIShaderHandle createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIShaderProgramHandle createShaderProgram(
		std::string name,
		const GHIInfoShaderSet& shaders) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIVertexStorageHandle createVertexStorage(
		const GHIInfoVertexGroupFormat& format,
		std::size_t numVertices,
		EGHIStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIIndexStorageHandle createIndexStorage(
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIMeshHandle createMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		GHIIndexStorageHandle indexStorage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteTexture(GHITextureHandle texture) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteFramebuffer(GHIFramebufferHandle framebuffer) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteShader(GHIShaderHandle shader) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteShaderProgram(GHIShaderProgramHandle shaderProgram) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteVertexStorage(GHIVertexStorageHandle vertexStorage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteIndexStorage(GHIIndexStorageHandle indexStorage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteMesh(GHIMeshHandle mesh) = 0;

public:
	/*!
	@note Thread safe.
	*/
	GHITextureHandle createTexture1D(
		const GHIInfoTextureFormat& format,
		uint32 lengthPx);

	/*!
	@note Thread safe.
	*/
	GHITextureHandle createTexture2D(
		const GHIInfoTextureFormat& format,
		const math::Vector2UI& sizePx);

	/*!
	@note Thread safe.
	*/
	GHITextureHandle createTexture3D(
		const GHIInfoTextureFormat& format,
		const math::Vector3UI& sizePx);

	/*!
	@note Thread safe.
	*/
	TAtomicQueue<GraphicsObjectCreater>& getCreateQueue();

	/*!
	@note Thread safe.
	*/
	TAtomicQueue<GraphicsObjectDeleter>& getDeleteQueue();

	void setGHIThread(GHIThread* thread);

private:
	GHIThread* m_ghiThread = nullptr;
	TAtomicQueue<GraphicsObjectCreater> m_createQueue;
	TAtomicQueue<GraphicsObjectDeleter> m_deleteQueue;
};

inline GHITextureHandle GraphicsObjectManager::createTexture1D(
	const GHIInfoTextureFormat& format,
	const uint32 lengthPx)
{
	return createTexture(format, {1, 1, 1});
}

inline GHITextureHandle GraphicsObjectManager::createTexture2D(
	const GHIInfoTextureFormat& format,
	const math::Vector2UI& sizePx)
{
	return createTexture(format, {sizePx.x(), sizePx.y(), 1});
}

inline GHITextureHandle GraphicsObjectManager::createTexture3D(
	const GHIInfoTextureFormat& format,
	const math::Vector3UI& sizePx)
{
	return createTexture(format, sizePx);
}

inline TAtomicQueue<GraphicsObjectCreater>& GraphicsObjectManager::getCreateQueue()
{
	return m_createQueue;
}

inline TAtomicQueue<GraphicsObjectDeleter>& GraphicsObjectManager::getDeleteQueue()
{
	return m_deleteQueue;
}

}// end namespace ph::editor