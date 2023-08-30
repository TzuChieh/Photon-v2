#pragma once

#include "RenderCore/GHI.h"

#include "ThirdParty/GLFW3_fwd.h"

#include <Common/config.h>

#include <thread>
#include <memory>

namespace ph::editor
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
	void draw(GHIMesh& mesh, EGHIMeshDrawMode drawMode) override;
	void swapBuffers() override;

	std::shared_ptr<GHITexture2D> createTexture2D(
		const GHIInfoTextureFormat& format,
		const math::Vector2UI& sizePx) override;

	std::shared_ptr<GHIFramebuffer> createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) override;

	std::shared_ptr<GHIShader> createShader(
		std::string name,
		EGHIShadingStage shadingStage,
		std::string shaderSource) override;

	std::shared_ptr<GHIShaderProgram> createShaderProgram(
		std::string name,
		const GHIShaderSet& shaders) override;

	std::shared_ptr<GHIVertexStorage> createVertexStorage(
		const GHIInfoVertexGroupFormat& format,
		std::size_t numVertices,
		EGHIStorageUsage usage) override;

	std::shared_ptr<GHIIndexStorage> createIndexStorage(
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage) override;

	std::shared_ptr<GHIMesh> createMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
		const std::shared_ptr<GHIIndexStorage>& indexStorage) override;

	GHIInfoDeviceCapability getDeviceCapabilities() override;

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
	std::unique_ptr<GHIInfoDeviceCapability> m_deviceCapability;
};

}// end namespace ph::editor
