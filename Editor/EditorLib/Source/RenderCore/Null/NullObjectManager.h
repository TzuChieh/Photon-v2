#pragma once

#include "RenderCore/GraphicsObjectManager.h"

namespace ph::editor::ghi
{

class NullObjectManager : public GraphicsObjectManager
{
public:
	~NullObjectManager() override = default;

	TextureHandle createTexture(const TextureDesc& desc) override
	{
		return TextureHandle{};
	}

	FramebufferHandle createFramebuffer(
		const FramebufferDesc& desc) override
	{
		return FramebufferHandle{};
	}

	ShaderHandle createShader(
		std::string name,
		EShadingStage shadingStage,
		std::string shaderSource) override
	{
		return ShaderHandle{};
	}

	ShaderProgramHandle createShaderProgram(
		std::string name,
		const ShaderSetInfo& shaders) override
	{
		return ShaderProgramHandle{};
	}

	VertexStorageHandle createVertexStorage(
		std::size_t numVertices,
		const VertexGroupFormatInfo& format,
		EStorageUsage usage) override
	{
		return VertexStorageHandle{};
	}

	IndexStorageHandle createIndexStorage(
		std::size_t numIndices,
		EStorageElement indexType,
		EStorageUsage usage) override
	{
		return IndexStorageHandle{};
	}

	MeshHandle createMesh(
		TSpanView<VertexStorageHandle> vertexStorages,
		const MeshVertexLayoutInfo& layout,
		IndexStorageHandle indexStorage) override
	{
		return MeshHandle{};
	}

	void uploadPixelData(
		TextureHandle handle,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) override
	{}

	void removeTexture(TextureHandle handle) override
	{}

	void removeFramebuffer(FramebufferHandle handle) override
	{}

	void removeShader(ShaderHandle handle) override
	{}

	void removeShaderProgram(ShaderProgramHandle handle) override
	{}

	void removeVertexStorage(VertexStorageHandle handle) override
	{}

	void removeIndexStorage(IndexStorageHandle handle) override
	{}

	void removeMesh(MeshHandle handle) override
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

}// end namespace ph::editor::ghi
