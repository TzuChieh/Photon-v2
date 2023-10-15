#pragma once

#include "RenderCore/GHI.h"

namespace ph::editor::ghi
{

class NullGHI : public GHI
{
public:
	NullGHI()
		: GHI(EGraphicsAPI::Unknown)
	{}

	~NullGHI() override = default;

	void load() override
	{}

	void unload() override
	{}

	void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) override
	{}

	void clearBuffer(EClearTarget targets) override
	{}

	void setClearColor(const math::Vector4F& color) override
	{}

	void draw(Mesh& mesh, EMeshDrawMode drawMode) override
	{}

	void swapBuffers() override
	{}

	bool tryUploadPixelData(
		TextureHandle handle,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) override
	{
		return false;
	}

	bool tryUploadPixelDataTo2DRegion(
		TextureHandle handle,
		const math::Vector2UI& regionOriginPx,
		const math::Vector2UI& regionSizePx,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) override
	{
		return false;
	}

	TextureNativeHandle tryGetTextureNativeHandle(TextureHandle handle) override
	{
		return {};
	}

	void attachTextureToFramebuffer(
		uint32 attachmentIdx,
		TextureHandle textureHandle,
		FramebufferHandle framebufferHandle) override
	{}

	std::shared_ptr<Shader> createShader(
		std::string name,
		EShadingStage shadingStage,
		std::string shaderSource) override
	{
		return nullptr;
	}

	std::shared_ptr<ShaderProgram> createShaderProgram(
		std::string name,
		const ShaderSetInfo& shaders) override
	{
		return nullptr;
	}

	std::shared_ptr<VertexStorage> createVertexStorage(
		const VertexGroupFormatInfo& format,
		std::size_t numVertices,
		EStorageUsage usage) override
	{
		return nullptr;
	}

	std::shared_ptr<IndexStorage> createIndexStorage(
		EStorageElement indexType,
		std::size_t numIndices,
		EStorageUsage usage) override
	{
		return nullptr;
	}

	std::shared_ptr<Mesh> createMesh(
		const MeshVertexLayoutInfo& layout,
		TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
		const std::shared_ptr<IndexStorage>& indexStorage) override
	{
		return nullptr;
	}

private:
	void beginRawCommand() override
	{}

	void endRawCommand() override
	{}
};

}// end namespace ph::editor::ghi
