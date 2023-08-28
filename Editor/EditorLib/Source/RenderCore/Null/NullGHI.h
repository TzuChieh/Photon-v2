#pragma once

#include "RenderCore/GHI.h"

namespace ph::editor
{

class NullGHI : public GHI
{
public:
	inline NullGHI()
		: GHI(EGraphicsAPI::Unknown)
	{}

	inline ~NullGHI() override = default;

	inline void load() override
	{}

	inline void unload() override
	{}

	inline void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) override
	{}

	inline void clearBuffer(EClearTarget targets) override
	{}

	inline void setClearColor(const math::Vector4F& color) override
	{}

	inline void draw(GHIMesh& mesh, EGHIMeshDrawMode drawMode) override
	{}

	inline void swapBuffers() override
	{}

	inline std::shared_ptr<GHITexture2D> createTexture2D(
		const GHIInfoTextureFormat& format,
		const math::Vector2UI& sizePx) override
	{
		return nullptr;
	}

	inline std::shared_ptr<GHIFramebuffer> createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) override
	{
		return nullptr;
	}

	inline std::shared_ptr<GHIShader> createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) override
	{
		return nullptr;
	}

	inline std::shared_ptr<GHIShaderProgram> createShaderProgram(
		std::string name,
		const GHIShaderSet& shaders) override
	{
		return nullptr;
	}

	inline std::shared_ptr<GHIVertexStorage> createVertexStorage(
		const GHIInfoVertexGroupFormat& format,
		std::size_t numVertices,
		EGHIStorageUsage usage) override
	{
		return nullptr;
	}

	inline std::shared_ptr<GHIIndexStorage> createIndexStorage(
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage) override
	{
		return nullptr;
	}

	inline std::shared_ptr<GHIMesh> createMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
		const std::shared_ptr<GHIIndexStorage>& indexStorage) override
	{
		return nullptr;
	}

private:
	inline void beginRawCommand() override
	{}

	inline void endRawCommand() override
	{}
};

}// end namespace ph::editor
