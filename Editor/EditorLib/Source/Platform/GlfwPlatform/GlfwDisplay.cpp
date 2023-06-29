#include "Platform/GlfwPlatform/GlfwDisplay.h"
#include "RenderCore/OpenGL/OpenglGHI.h"
#include "Platform/Platform.h"
#include "ThirdParty/GLFW3.h"
#include "App/Editor.h"
#include "EditorCore/Event/DisplayFramebufferResizedEvent.h"
#include "EditorCore/Event/DisplayClosedEvent.h"
#include "EditorCore/Event/DisplayFocusChangedEvent.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/logging.h>
#include <Common/assertion.h>
#include <Utility/utility.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwDisplay, EditorPlatform);

GlfwDisplay::GlfwDisplay()
	: PlatformDisplay()
	, m_glfwWindow(nullptr)
	, m_ghi(nullptr)
	, m_apiType(EGraphicsAPI::Unknown)
	, m_sizePx(0, 0)
{}

GlfwDisplay::~GlfwDisplay() = default;

void GlfwDisplay::initialize(
	Editor&            editor,
	const std::string& windowTitle,
	math::Vector2S     sizePx,
	EGraphicsAPI       graphicsApi,
	const bool         useDebugModeGHI)
{
	if(m_glfwWindow)
	{
		throw PlatformException(
			"cannot create window when the old one is not destroyed first");
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

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

	if(useDebugModeGHI)
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
	}

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

	// Note that although GLFW window is now successfully created, this does not mean the context 
	// is already current on main thread. We leave it to GHI to decide.

	glfwSetFramebufferSizeCallback(m_glfwWindow, 
		[](GLFWwindow* window, int width, int height)
		{
			Editor& editor = *(static_cast<Editor*>(glfwGetWindowUserPointer(window)));

			DisplayFramebufferResizedEvent e(width, height);
			editor.postEvent(e, editor.onDisplayFramebufferResized);
		});

	glfwSetWindowCloseCallback(m_glfwWindow,
		[](GLFWwindow* window)
		{
			Editor& editor = *(static_cast<Editor*>(glfwGetWindowUserPointer(window)));

			DisplayClosedEvent e;
			editor.postEvent(e, editor.onDisplayClosed);
		});

	glfwSetWindowFocusCallback(m_glfwWindow, 
		[](GLFWwindow* window, int focused)
		{
			Editor& editor = *(static_cast<Editor*>(glfwGetWindowUserPointer(window)));

			DisplayFocusChangedEvent e(focused == GLFW_TRUE);
			editor.postEvent(e, editor.onDisplayFocusChanged);
		});

	PH_ASSERT(!m_ghi);
	if(graphicsApi == EGraphicsAPI::OpenGL)
	{
		m_ghi = std::make_unique<OpenglGHI>(m_glfwWindow, useDebugModeGHI);
	}
	
	m_apiType = graphicsApi;
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
	
	// If this fails, most likely GHI was not unloaded; otherwise, the context must be set incorrectly 
	// by someone. 
	PH_ASSERT(!glfwGetCurrentContext());

	// Destroys the GLFW window. As GLFW doc notes, the context of the specified window must not be 
	// current on any other thread when `glfwDestroyWindow()` is called. In our case, GHI must have 
	// been unloaded by other routines.
	glfwDestroyWindow(m_glfwWindow);
}

GHI* GlfwDisplay::getGHI() const
{
	PH_ASSERT(m_ghi);
	return m_ghi.get();
}

EGraphicsAPI GlfwDisplay::getGraphicsApiType() const
{
	PH_ASSERT(m_apiType != EGraphicsAPI::Unknown);
	return m_apiType;
}

float32 GlfwDisplay::getDpiScale() const
{
	GLFWmonitor* const primaryMonitor = glfwGetPrimaryMonitor();
	if(primaryMonitor)
	{
		float xscale, yscale;
		glfwGetMonitorContentScale(primaryMonitor, &xscale, &yscale);

		// Note: Utilize `xscale` only. Currently we cannot handle non-rigid scaling.
		return xscale;
	}
	else
	{
		return 1.0f;
	}
}

math::Vector2S GlfwDisplay::getFramebufferSizePx() const
{
	PH_ASSERT(m_glfwWindow);

	// The precondition required by `glfwGetFramebufferSize()`
	PH_ASSERT(Threads::isOnMainThread());

	int width, height;
	glfwGetFramebufferSize(m_glfwWindow, &width, &height);

	return math::Vector2S(
		lossless_cast<std::size_t>(width),
		lossless_cast<std::size_t>(height));
}

GlfwDisplay::NativeWindow GlfwDisplay::getNativeWindow() const
{
	PH_ASSERT(m_glfwWindow);
	return m_glfwWindow;
}

}// end namespace ph::editor
