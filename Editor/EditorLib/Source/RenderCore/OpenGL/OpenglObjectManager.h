#pragma once

#include "RenderCore/GraphicsObjectManager.h"

namespace ph::editor
{

class OpenglObjectManager : public GraphicsObjectManager
{
public:
	~OpenglObjectManager() override;

	inline GHITextureHandle createTexture(
		const GHIInfoTextureFormat& format,
		const math::Vector3UI& sizePx) override
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
		const GHIInfoVertexGroupFormat& format,
		std::size_t numVertices,
		EGHIStorageUsage usage) override
	{
		return GHIVertexStorageHandle{};
	}

	inline GHIIndexStorageHandle createIndexStorage(
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage) override
	{
		return GHIIndexStorageHandle{};
	}

	inline GHIMeshHandle createMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		GHIIndexStorageHandle indexStorage) override
	{
		return GHIMeshHandle{};
	}

	inline void deleteTexture(GHITextureHandle texture) override
	{}

	inline void deleteFramebuffer(GHIFramebufferHandle framebuffer) override
	{}

	inline void deleteShader(GHIShaderHandle shader) override
	{}

	inline void deleteShaderProgram(GHIShaderProgramHandle shaderProgram) override
	{}

	inline void deleteVertexStorage(GHIVertexStorageHandle vertexStorage) override
	{}

	inline void deleteIndexStorage(GHIIndexStorageHandle indexStorage) override
	{}

	inline void deleteMesh(GHIMeshHandle mesh) override
	{}
};

}// end namespace ph::editor
