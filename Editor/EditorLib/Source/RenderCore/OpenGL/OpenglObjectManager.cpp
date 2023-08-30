#include "RenderCore/OpenGL/OpenglObjectManager.h"
#include "RenderCore/OpenGL/OpenglContext.h"

namespace ph::editor
{

OpenglObjectManager::OpenglObjectManager(OpenglContext& ctx)
	: GraphicsObjectManager()
	, m_ctx(ctx)
{}

OpenglObjectManager::~OpenglObjectManager() = default;

GHITextureHandle OpenglObjectManager::createTexture(
	const GHIInfoTextureFormat& format,
	const math::Vector3UI& sizePx)
{
	return GHITextureHandle{};
}

GHIFramebufferHandle OpenglObjectManager::createFramebuffer(
	const GHIInfoFramebufferAttachment& attachments)
{
	return GHIFramebufferHandle{};
}

GHIShaderHandle OpenglObjectManager::createShader(
	std::string name,
	EGHIShadingStage shadingStage,
	std::string shaderSource)
{
	return GHIShaderHandle{};
}

GHIShaderProgramHandle OpenglObjectManager::createShaderProgram(
	std::string name,
	const GHIInfoShaderSet& shaders)
{
	return GHIShaderProgramHandle{};
}

GHIVertexStorageHandle OpenglObjectManager::createVertexStorage(
	const GHIInfoVertexGroupFormat& format,
	std::size_t numVertices,
	EGHIStorageUsage usage)
{
	return GHIVertexStorageHandle{};
}

GHIIndexStorageHandle OpenglObjectManager::createIndexStorage(
	EGHIStorageElement indexType,
	std::size_t numIndices,
	EGHIStorageUsage usage)
{
	return GHIIndexStorageHandle{};
}

GHIMeshHandle OpenglObjectManager::createMesh(
	const GHIInfoMeshVertexLayout& layout,
	TSpanView<GHIVertexStorageHandle> vertexStorages,
	GHIIndexStorageHandle indexStorage)
{
	return GHIMeshHandle{};
}

void OpenglObjectManager::deleteTexture(GHITextureHandle texture)
{}

void OpenglObjectManager::deleteFramebuffer(GHIFramebufferHandle framebuffer)
{}

void OpenglObjectManager::deleteShader(GHIShaderHandle shader)
{}

void OpenglObjectManager::deleteShaderProgram(GHIShaderProgramHandle shaderProgram)
{}

void OpenglObjectManager::deleteVertexStorage(GHIVertexStorageHandle vertexStorage)
{}

void OpenglObjectManager::deleteIndexStorage(GHIIndexStorageHandle indexStorage)
{}

void OpenglObjectManager::deleteMesh(GHIMeshHandle mesh)
{}

}// end namespace ph::editor
