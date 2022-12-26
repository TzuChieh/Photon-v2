#pragma once

#include "RenderCore/GHI.h"
#include "ThirdParty/GLFW3_fwd.h"

#include <Common/config.h>

#include <thread>
#include <memory>

namespace ph::editor
{

class OpenglGHI : public GHI
{
public:
	OpenglGHI(GLFWwindow* glfwWindow, bool hasDebugContext);
	~OpenglGHI() override;

	void load() override;
	void unload() override;
	void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) override;
	void clearBuffer(EClearTarget targets) override;
	void setClearColor(const math::Vector4F& color) override;
	void draw(GHIMeshStorage& meshStorage, EGHIInfoMeshDrawMode drawMode) override;
	void swapBuffers() override;

	std::shared_ptr<GHITexture2D> createTexture2D(
		const GHIInfoTextureFormat& format,
		const math::Vector2UI& sizePx) override;

	std::shared_ptr<GHIFramebuffer> createFramebuffer(
		const GHIInfoFramebufferAttachment& attachments) override;

	GHIInfoDeviceCapability getDeviceCapabilities() override;

private:
	void beginRawCommand() override;
	void endRawCommand() override;

private:
	GLFWwindow* m_glfwWindow;
	bool m_hasDebugContext;
	bool m_isLoaded;
#ifdef PH_DEBUG
	std::thread::id m_loadThreadId;
#endif
	std::unique_ptr<GHIInfoDeviceCapability> m_deviceCapability;
};

}// end namespace ph::editor