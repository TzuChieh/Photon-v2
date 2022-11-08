#pragma once

#include "RenderCore/GHI.h"
#include "ThirdParty/GLFW3_fwd.h"

#include <Common/config.h>

#include <thread>

namespace ph::editor
{

class GlfwGladOpenglGHI : public GHI
{
public:
	explicit GlfwGladOpenglGHI(GLFWwindow* glfwWindow);
	~GlfwGladOpenglGHI() override;

	void load() override;
	void unload() override;
	void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) override;
	void swapBuffers() override;

private:
	void beginRawCommand() override;
	void endRawCommand() override;

private:
	GLFWwindow*     m_glfwWindow;
	bool            m_isLoaded;
#ifdef PH_DEBUG
	std::thread::id m_loadThreadId;
#endif
};

}// end namespace ph::editor
