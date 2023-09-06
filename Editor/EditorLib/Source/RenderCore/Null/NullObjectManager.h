#pragma once

#include "RenderCore/GraphicsObjectManager.h"

namespace ph::editor
{

class NullObjectManager : public GraphicsObjectManager
{
public:
	inline ~NullObjectManager() override = default;

	inline GHITextureHandle createTexture(const GHIInfoTextureDesc& desc) override
	{
		return GHITextureHandle{};
	}

	inline GHIFramebufferHandle createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) override
	{
		return GHIFramebufferHandle{};
	}

	inline GHIShaderHandle createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) override
	{
		return GHIShaderHandle{};
	}

	inline GHIShaderProgramHandle createShaderProgram(
		std::string name,
		const GHIInfoShaderSet& shaders) override
	{
		return GHIShaderProgramHandle{};
	}

	inline GHIVertexStorageHandle createVertexStorage(
		std::size_t numVertices,
		const GHIInfoVertexGroupFormat& format,
		EGHIStorageUsage usage) override
	{
		return GHIVertexStorageHandle{};
	}

	inline GHIIndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EGHIStorageElement indexType,
		EGHIStorageUsage usage) override
	{
		return GHIIndexStorageHandle{};
	}

	inline GHIMeshHandle createMesh(
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		const GHIInfoMeshVertexLayout& layout,
		GHIIndexStorageHandle indexStorage) override
	{
		return GHIMeshHandle{};
	}

	inline void deleteTexture(GHITextureHandle handle) override
	{}

	inline void deleteFramebuffer(GHIFramebufferHandle handle) override
	{}

	inline void deleteShader(GHIShaderHandle handle) override
	{}

	inline void deleteShaderProgram(GHIShaderProgramHandle handle) override
	{}

	inline void deleteVertexStorage(GHIVertexStorageHandle handle) override
	{}

	inline void deleteIndexStorage(GHIIndexStorageHandle handle) override
	{}

	inline void deleteMesh(GHIMeshHandle handle) override
	{}

	inline void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}

	inline void endFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}
};

}// end namespace ph::editor
