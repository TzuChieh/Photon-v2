#include "Platform/GlfwPlatform/GlfwDisplay.h"
#include "RenderCore/OpenGL/OpenglContext.h"
#include "Platform/Platform.h"
#include "App/Editor.h"
#include "EditorCore/Event/DisplayFramebufferResizedEvent.h"
#include "EditorCore/Event/DisplayClosedEvent.h"
#include "EditorCore/Event/DisplayFocusChangedEvent.h"
#include "EditorCore/Thread/Threads.h"

#include "ThirdParty/GLFW3.h"

#include <Common/logging.h>
#include <Common/assertion.h>
#include <Common/utility.h>
#include <Common/os.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwDisplay, EditorPlatform);

GlfwDisplay::GlfwDisplay()
	: PlatformDisplay()
	, m_glfwWindow(nullptr)
	, m_graphicsCtx(nullptr)
	, m_apiType(ghi::EGraphicsAPI::Unknown)
	, m_sizePx(0, 0)
{}

GlfwDisplay::~GlfwDisplay() = default;

void GlfwDisplay::initialize(
	Editor&            editor,
	const std::string& windowTitle,
	math::Vector2S     sizePx,
	ghi::EGraphicsAPI  graphicsAPI,
	const bool         useDebugModeGHI)
{
	if(m_glfwWindow)
	{
		throw PlatformException(
			"cannot create window when the old one is not destroyed first");
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	if(graphicsAPI == ghi::EGraphicsAPI::OpenGL)
	{
		// We need at least OpenGL 4.5 for DSA
		int majorVersion = 4;
		int minorVersion = 6;
		if constexpr(PH_OPERATING_SYSTEM_IS_LINUX)
		{
			// FIXME: this is basically due to my hardware limitation; should query for max supported
			// version instead
			minorVersion = 5;
		}

		PH_LOG(GlfwDisplay, Note, "target graphics API: OpenGL, requesting version {}.{}",
			majorVersion, minorVersion);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
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

	PH_ASSERT(!m_graphicsCtx);
	if(graphicsAPI == ghi::EGraphicsAPI::OpenGL)
	{
		m_graphicsCtx = std::make_unique<ghi::OpenglContext>(m_glfwWindow, useDebugModeGHI);
	}
	
	m_apiType = graphicsAPI;
	m_sizePx = sizePx;
}

void GlfwDisplay::terminate()
{
	if(!m_glfwWindow)
	{
		return;
	}

	glfwSetFramebufferSizeCallback(m_glfwWindow, nullptr);

	PH_ASSERT(m_graphicsCtx);
	
	// If this fails, most likely GHI was not unloaded; otherwise, the context must be set incorrectly 
	// by someone. 
	PH_ASSERT(!glfwGetCurrentContext());

	// Destroys the GLFW window. As GLFW doc notes, the context of the specified window must not be 
	// current on any other thread when `glfwDestroyWindow()` is called. In our case, GHI must have 
	// been unloaded by other routines.
	glfwDestroyWindow(m_glfwWindow);
}

ghi::GraphicsContext* GlfwDisplay::getGraphicsContext() const
{
	PH_ASSERT(m_graphicsCtx);
	return m_graphicsCtx.get();
}

ghi::EGraphicsAPI GlfwDisplay::getGraphicsApiType() const
{
	PH_ASSERT(m_apiType != ghi::EGraphicsAPI::Unknown);
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
