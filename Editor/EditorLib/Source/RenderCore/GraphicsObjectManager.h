#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>
#include <Utility/TSpan.h>

#include <cstddef>
#include <string>

namespace ph::editor { class GHIThreadUpdateContext; }

namespace ph::editor::ghi
{

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
	virtual TextureHandle createTexture(const TextureDesc& desc) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual FramebufferHandle createFramebuffer(
		const FramebufferDesc& desc) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual ShaderHandle createShader(
		std::string name,
		EShadingStage shadingStage,
		std::string shaderSource) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual ShaderProgramHandle createShaderProgram(
		std::string name,
		const ShaderSetInfo& shaders) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual VertexStorageHandle createVertexStorage(
		std::size_t numVertices,
		const VertexGroupFormatInfo& format,
		EStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual IndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EStorageElement indexType,
		EStorageUsage usage) = 0;

	/*!
	@note Thread safe.
	*/
	[[nodiscard]]
	virtual MeshHandle createMesh(
		TSpanView<VertexStorageHandle> vertexStorages,
		const MeshVertexLayoutInfo& layout,
		IndexStorageHandle indexStorage) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void uploadPixelData(
		TextureHandle handle,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeTexture(TextureHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeFramebuffer(FramebufferHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeShader(ShaderHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeShaderProgram(ShaderProgramHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeVertexStorage(VertexStorageHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeIndexStorage(IndexStorageHandle handle) = 0;

	/*!
	@note Thread safe.
	*/
	virtual void removeMesh(MeshHandle handle) = 0;

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

}// end namespace ph::editor::ghi
