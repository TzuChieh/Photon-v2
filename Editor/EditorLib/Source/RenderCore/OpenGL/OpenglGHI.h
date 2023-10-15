#pragma once

#include "RenderCore/GHI.h"

#include "ThirdParty/GLFW3_fwd.h"

#include <Common/config.h>

#include <thread>
#include <memory>

namespace ph::editor::ghi
{

class OpenglContext;

class OpenglGHI final : public GHI
{
public:
	OpenglGHI(OpenglContext& ctx, GLFWwindow* glfwWindow, bool hasDebugContext);
	~OpenglGHI() override;

	void load() override;
	void unload() override;
	void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) override;
	void clearBuffer(EClearTarget targets) override;
	void setClearColor(const math::Vector4F& color) override;
	void draw(Mesh& mesh, EMeshDrawMode drawMode) override;
	void swapBuffers() override;

	bool tryUploadPixelData(
		TextureHandle handle,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) override;

	bool tryUploadPixelDataTo2DRegion(
		TextureHandle handle,
		const math::Vector2UI& regionOriginPx,
		const math::Vector2UI& regionSizePx,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) override;

	TextureNativeHandle tryGetTextureNativeHandle(TextureHandle handle) override;

	void attachTextureToFramebuffer(
		uint32 attachmentIdx,
		TextureHandle textureHandle,
		FramebufferHandle framebufferHandle) override;

	std::shared_ptr<Shader> createShader(
		std::string name,
		EShadingStage shadingStage,
		std::string shaderSource) override;

	std::shared_ptr<ShaderProgram> createShaderProgram(
		std::string name,
		const ShaderSetInfo& shaders) override;

	std::shared_ptr<VertexStorage> createVertexStorage(
		const VertexGroupFormatInfo& format,
		std::size_t numVertices,
		EStorageUsage usage) override;

	std::shared_ptr<IndexStorage> createIndexStorage(
		EStorageElement indexType,
		std::size_t numIndices,
		EStorageUsage usage) override;

	std::shared_ptr<Mesh> createMesh(
		const MeshVertexLayoutInfo& layout,
		TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
		const std::shared_ptr<IndexStorage>& indexStorage) override;

	DeviceCapabilityInfo getDeviceCapabilities() override;

private:
	void beginRawCommand() override;
	void endRawCommand() override;

private:
	OpenglContext& m_ctx;
	GLFWwindow* m_glfwWindow;
	bool m_hasDebugContext;
	bool m_isLoaded;
#if PH_DEBUG
	std::thread::id m_loadThreadId;
#endif
	std::unique_ptr<DeviceCapabilityInfo> m_deviceCapability;
};

}// end namespace ph::editor::ghi
