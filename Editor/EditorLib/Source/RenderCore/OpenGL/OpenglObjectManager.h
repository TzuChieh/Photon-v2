#pragma once

#include "RenderCore/GraphicsObjectManager.h"
#include "EditorCore/Storage/TTrivialItemPool.h"

namespace ph::editor
{

class OpenglContext;

class OpenglObjectManager final : public GraphicsObjectManager
{
public:
	explicit OpenglObjectManager(OpenglContext& ctx);
	~OpenglObjectManager() override;

	GHITextureHandle createTexture(
		const GHIInfoTextureFormat& format,
		const math::Vector3UI& sizePx) override;

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
		const GHIInfoVertexGroupFormat& format,
		std::size_t numVertices,
		EGHIStorageUsage usage) override;

	GHIIndexStorageHandle createIndexStorage(
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage) override;

	GHIMeshHandle createMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		GHIIndexStorageHandle indexStorage) override;

	void deleteTexture(GHITextureHandle texture) override;
	void deleteFramebuffer(GHIFramebufferHandle framebuffer) override;
	void deleteShader(GHIShaderHandle shader) override;
	void deleteShaderProgram(GHIShaderProgramHandle shaderProgram) override;
	void deleteVertexStorage(GHIVertexStorageHandle vertexStorage) override;
	void deleteIndexStorage(GHIIndexStorageHandle indexStorage) override;
	void deleteMesh(GHIMeshHandle mesh) override;

private:
	OpenglContext& m_ctx;
};

}// end namespace ph::editor
