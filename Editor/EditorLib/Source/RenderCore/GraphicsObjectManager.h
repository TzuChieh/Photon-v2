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

class GraphicsObjectManager
{
public:
	virtual ~GraphicsObjectManager();

	virtual GHITextureHandle createTexture(
		const GHIInfoTextureFormat& format,
		const math::Vector3UI& sizePx) = 0;

	virtual GHIFramebufferHandle createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) = 0;

	virtual GHIShaderHandle createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) = 0;

	virtual GHIShaderProgramHandle createShaderProgram(
		std::string name,
		const GHIInfoShaderSet& shaders) = 0;

	virtual GHIVertexStorageHandle createVertexStorage(
		const GHIInfoVertexGroupFormat& format,
		std::size_t numVertices,
		EGHIStorageUsage usage) = 0;

	virtual GHIIndexStorageHandle createIndexStorage(
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage) = 0;

	virtual GHIMeshHandle createMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<GHIVertexStorageHandle> vertexStorages,
		GHIIndexStorageHandle indexStorage) = 0;

	virtual void deleteTexture(GHITextureHandle texture) = 0;
	virtual void deleteFramebuffer(GHIFramebufferHandle framebuffer) = 0;
	virtual void deleteShader(GHIShaderHandle shader) = 0;
	virtual void deleteShaderProgram(GHIShaderProgramHandle shaderProgram) = 0;
	virtual void deleteVertexStorage(GHIVertexStorageHandle vertexStorage) = 0;
	virtual void deleteIndexStorage(GHIIndexStorageHandle indexStorage) = 0;
	virtual void deleteMesh(GHIMeshHandle mesh) = 0;

public:
	GHITextureHandle createTexture1D(
		const GHIInfoTextureFormat& format,
		uint32 lengthPx);

	GHITextureHandle createTexture2D(
		const GHIInfoTextureFormat& format,
		const math::Vector2UI& sizePx);

	GHITextureHandle createTexture3D(
		const GHIInfoTextureFormat& format,
		const math::Vector3UI& sizePx);
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

}// end namespace ph::editor
