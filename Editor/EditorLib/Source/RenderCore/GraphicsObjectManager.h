#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>
#include <Utility/TSpan.h>

#include <cstddef>
#include <string>

namespace ph::editor
{

class GHIThread;

/*! @brief Manages the creation and deletion of graphics-related resource objects.
All `create<XXX>()` and `delete<XXX>()` methods are thread safe as long as they are called within
the lifetime of current graphics context, and implementations must follow these contracts strictly.
*/
class GraphicsObjectManager
{
public:
	virtual ~GraphicsObjectManager();

	/*!
	@note Thread safe.
	*/
	virtual GHITextureHandle createTexture(const GHIInfoTextureDesc& desc) = 0;

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
		std::size_t numVertices,
		const GHIInfoVertexGroupFormat& format,
		EGHIStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIIndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EGHIStorageElement indexType,
		EGHIStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual GHIMeshHandle createMesh(
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		const GHIInfoMeshVertexLayout& layout,
		GHIIndexStorageHandle indexStorage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteTexture(GHITextureHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteFramebuffer(GHIFramebufferHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteShader(GHIShaderHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteShaderProgram(GHIShaderProgramHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteVertexStorage(GHIVertexStorageHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteIndexStorage(GHIIndexStorageHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void deleteMesh(GHIMeshHandle handle) = 0;

	/*! @brief Called on GHI thread when a frame begins.
	*/
	virtual void beginFrameUpdate() = 0;

	/*! @brief Called on GHI thread when a frame ends.
	*/
	virtual void endFrameUpdate() = 0;
};

}// end namespace ph::editor
