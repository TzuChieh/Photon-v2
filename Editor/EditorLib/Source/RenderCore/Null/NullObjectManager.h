#pragma once

#include "RenderCore/GraphicsObjectManager.h"

namespace ph::editor
{

class NullObjectManager : public GraphicsObjectManager
{
public:
	~NullObjectManager() override = default;

	GHITextureHandle createTexture(const GHIInfoTextureDesc& desc) override
	{
		return GHITextureHandle{};
	}

	GHIFramebufferHandle createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) override
	{
		return GHIFramebufferHandle{};
	}

	GHIShaderHandle createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) override
	{
		return GHIShaderHandle{};
	}

	GHIShaderProgramHandle createShaderProgram(
		std::string name,
		const GHIInfoShaderSet& shaders) override
	{
		return GHIShaderProgramHandle{};
	}

	GHIVertexStorageHandle createVertexStorage(
		std::size_t numVertices,
		const GHIInfoVertexGroupFormat& format,
		EGHIStorageUsage usage) override
	{
		return GHIVertexStorageHandle{};
	}

	GHIIndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EGHIStorageElement indexType,
		EGHIStorageUsage usage) override
	{
		return GHIIndexStorageHandle{};
	}

	GHIMeshHandle createMesh(
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		const GHIInfoMeshVertexLayout& layout,
		GHIIndexStorageHandle indexStorage) override
	{
		return GHIMeshHandle{};
	}

	void uploadPixelData(
		GHITextureHandle handle,
		TSpanView<std::byte> pixelData,
		EGHIPixelComponent componentType) override
	{}

	void deleteTexture(GHITextureHandle handle) override
	{}

	void deleteFramebuffer(GHIFramebufferHandle handle) override
	{}

	void deleteShader(GHIShaderHandle handle) override
	{}

	void deleteShaderProgram(GHIShaderProgramHandle handle) override
	{}

	void deleteVertexStorage(GHIVertexStorageHandle handle) override
	{}

	void deleteIndexStorage(GHIIndexStorageHandle handle) override
	{}

	void deleteMesh(GHIMeshHandle handle) override
	{}

	void onGHILoad() override
	{}

	void onGHIUnload() override
	{}

	void beginFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}

	void endFrameUpdate(const GHIThreadUpdateContext& ctx) override
	{}
};

}// end namespace ph::editor
