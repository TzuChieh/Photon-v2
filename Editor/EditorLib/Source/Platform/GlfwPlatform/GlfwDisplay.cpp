#include "Platform/GlfwPlatform/GlfwDisplay.h"
#include "RenderCore/OpenGL/GlfwGladOpenglGHI.h"
#include "Platform/Platform.h"
#include "ThirdParty/GLFW3.h"
#include "App/Editor.h"
#include "App/Event/FrameBufferResizeEvent.h"

#include <Common/logging.h>
#include <Common/assertion.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwDisplay, EditorPlatform);

GlfwDisplay::GlfwDisplay()
	: PlatformDisplay()
	, m_glfwWindow(nullptr)
	, m_ghi(nullptr)
	, m_sizePx(0, 0)
{}

GlfwDisplay::~GlfwDisplay() = default;

void GlfwDisplay::initialize(
	Editor&            editor,
	const std::string& windowTitle,
	math::Vector2S     sizePx,
	EGraphicsAPI       graphicsApi)
{
	if(m_glfwWindow)
	{
		throw PlatformException(
			"cannot create window when the old one is not destroyed first");
	}

	if(graphicsApi == EGraphicsAPI::OpenGL)
	{
		PH_LOG(GlfwDisplay, "target graphics API: OpenGL, requesting version 4.6");

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
	}
	else
	{
		throw PlatformException(
			"requesting unsupported graphics API");
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_glfwWindow = glfwCreateWindow(
		static_cast<int>(sizePx.x()), 
		static_cast<int>(sizePx.y()), 
		windowTitle.c_str(), 
		nullptr,
		nullptr);
	if(!m_glfwWindow)
	{
		throw_formatted<PlatformException>(
			"error creating window \"{}\" of size ({}, {})",
			windowTitle, sizePx.x(), sizePx.y());
	}

	glfwSetWindowUserPointer(m_glfwWindow, &editor);

	glfwSetFramebufferSizeCallback(m_glfwWindow, 
		[](GLFWwindow* window, int width, int height)
		{
			Editor& editor = *(static_cast<Editor*>(glfwGetWindowUserPointer(window)));

			FrameBufferResizeEvent e(math::Vector2S(width, height));
			editor.dispatchToEventQueue(e, editor.onFrameBufferResize);
		});

	PH_ASSERT(!m_ghi);
	m_ghi = std::make_unique<GlfwGladOpenglGHI>(m_glfwWindow);

	m_sizePx = sizePx;
}

void GlfwDisplay::terminate()
{
	if(!m_glfwWindow)
	{
		return;
	}

	glfwSetFramebufferSizeCallback(m_glfwWindow, nullptr);

	PH_ASSERT(m_ghi);

	// TODO: ensure rendering stopped
	// TODO: cleanup GHI
	// TODO: make context current on this thread

	// As GLFW doc notes, the context of the specified window must not be current on any other thread when
	// this function is called. In our case, GHI must be cleaned up and make the context current on main
	// thread again.
	glfwDestroyWindow(m_glfwWindow);
}

}// end namespace ph::editor
