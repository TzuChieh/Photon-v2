#pragma once

#include "RenderCore/GHI.h"

namespace ph::editor
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

	void draw(GHIMesh& mesh, EGHIMeshDrawMode drawMode) override
	{}

	void swapBuffers() override
	{}

	bool tryUploadPixelData(
		GHITextureHandle handle,
		TSpanView<std::byte> pixelData,
		EGHIPixelFormat pixelFormat,
		EGHIPixelComponent pixelComponent) override
	{
		return false;
	}

	GHITextureNativeHandle tryGetTextureNativeHandle(GHITextureHandle handle) override
	{
		return {};
	}

	std::shared_ptr<GHIFramebuffer> createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) override
	{
		return nullptr;
	}

	std::shared_ptr<GHIShader> createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) override
	{
		return nullptr;
	}

	std::shared_ptr<GHIShaderProgram> createShaderProgram(
		std::string name,
		const GHIShaderSet& shaders) override
	{
		return nullptr;
	}

	std::shared_ptr<GHIVertexStorage> createVertexStorage(
		const GHIInfoVertexGroupFormat& format,
		std::size_t numVertices,
		EGHIStorageUsage usage) override
	{
		return nullptr;
	}

	std::shared_ptr<GHIIndexStorage> createIndexStorage(
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage) override
	{
		return nullptr;
	}

	std::shared_ptr<GHIMesh> createMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
		const std::shared_ptr<GHIIndexStorage>& indexStorage) override
	{
		return nullptr;
	}

private:
	void beginRawCommand() override
	{}

	void endRawCommand() override
	{}
};

}// end namespace ph::editor
