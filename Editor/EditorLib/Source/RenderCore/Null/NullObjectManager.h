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
		const GHIInfoFramebufferDesc& desc) override
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
		EGHIPixelFormat pixelFormat,
		EGHIPixelComponent pixelComponent) override
	{}

	void removeTexture(GHITextureHandle handle) override
	{}

	void removeFramebuffer(GHIFramebufferHandle handle) override
	{}

	void removeShader(GHIShaderHandle handle) override
	{}

	void removeShaderProgram(GHIShaderProgramHandle handle) override
	{}

	void removeVertexStorage(GHIVertexStorageHandle handle) override
	{}

	void removeIndexStorage(GHIIndexStorageHandle handle) override
	{}

	void removeMesh(GHIMeshHandle handle) override
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
