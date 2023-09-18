#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>
#include <Utility/TSpan.h>

#include <cstddef>
#include <string>

namespace ph::editor
{

class GHIThreadUpdateContext;

/*! @brief Manages the creation and deletion of graphics-related resource objects.
All `create<XXX>()` and `delete<XXX>()` along with some data manipulation methods are thread safe
as long as they are called within the lifetime of current graphics context. During the lifetime of
the current graphics context, the `GHI` abstraction may be loaded/unloaded and thread safe methods
are allowed to fail gently (not crash or causing any data corruption) during the unloaded period.
*/
class GraphicsObjectManager
{
public:
	virtual ~GraphicsObjectManager();

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual GHITextureHandle createTexture(const GHIInfoTextureDesc& desc) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual GHIFramebufferHandle createFramebuffer(
		const GHIInfoFramebufferDesc& desc) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual GHIShaderHandle createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual GHIShaderProgramHandle createShaderProgram(
		std::string name,
		const GHIInfoShaderSet& shaders) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual GHIVertexStorageHandle createVertexStorage(
		std::size_t numVertices,
		const GHIInfoVertexGroupFormat& format,
		EGHIStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual GHIIndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EGHIStorageElement indexType,
		EGHIStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual GHIMeshHandle createMesh(
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		const GHIInfoMeshVertexLayout& layout,
		GHIIndexStorageHandle indexStorage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void uploadPixelData(
		GHITextureHandle handle,
		TSpanView<std::byte> pixelData,
		EGHIPixelFormat pixelFormat,
		EGHIPixelComponent pixelComponent) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeTexture(GHITextureHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeFramebuffer(GHIFramebufferHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeShader(GHIShaderHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeShaderProgram(GHIShaderProgramHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeVertexStorage(GHIVertexStorageHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeIndexStorage(GHIIndexStorageHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeMesh(GHIMeshHandle handle) = 0;

	/*! @brief Called by GHI thread after GHI is loaded.
	*/
	virtual void onGHILoad() = 0;

	/*! @brief Called by GHI thread before GHI is unloaded.
	*/
	virtual void onGHIUnload() = 0;

	/*! @brief Called by GHI thread when a frame begins.
	*/
	virtual void beginFrameUpdate(const GHIThreadUpdateContext& ctx) = 0;

	/*! @brief Called by GHI thread when a frame ends.
	*/
	virtual void endFrameUpdate(const GHIThreadUpdateContext& ctx) = 0;
};

}// end namespace ph::editor
