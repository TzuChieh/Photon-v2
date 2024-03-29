#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "App/AppSettings.h"

#include "ThirdParty/GLFW3.h"

#include <Common/logging.h>
#include <Common/assertion.h>
#include <Math/TVector2.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwPlatform, EditorPlatform);

namespace
{

inline void glfw_error_callback(const int errorCode, const char* const errorDescription)
{
	PH_LOG(GlfwPlatform, Error, "GLFW error: {} (error code = {})",
		errorDescription, errorCode);
}

}// end anonymous namespace

GlfwPlatform::GlfwPlatform(const AppSettings& settings, Editor& editor)
	: Platform(editor)
	, m_input()
	, m_display()
{
	initialize(settings);
}

GlfwPlatform::~GlfwPlatform()
{
	terminate();
}

void GlfwPlatform::update(const float64 deltaS)
{
	m_input.poll(deltaS);
}

void GlfwPlatform::initialize(const AppSettings& settings)
{
	glfwSetErrorCallback(glfw_error_callback);

	const int initReturnCode = glfwInit();
	if(initReturnCode != GLFW_TRUE)
	{
		throw PlatformException(
			"error occurred on initializing GLFW");
	}

	ghi::EGraphicsAPI requestedGraphicsAPI = settings.graphicsAPI;
	if(settings.graphicsAPI == ghi::EGraphicsAPI::Unknown)
	{
		PH_LOG(GlfwPlatform, Note,
			"graphics API defaults to OpenGL");

		requestedGraphicsAPI = ghi::EGraphicsAPI::OpenGL;
	}

	math::Vector2S requestedDisplaySizePx = settings.displaySizePx;
	if(requestedDisplaySizePx.product() == 0)
	{
		PH_LOG(GlfwPlatform, Note,
			"display area is 0, trying to request a default size");

		GLFWmonitor* const primaryMonitor = glfwGetPrimaryMonitor();
		if(!primaryMonitor)
		{
			requestedDisplaySizePx = {1920, 1080};

			PH_LOG(GlfwPlatform, Warning,
				"cannot find primary monitor, display size defaults to {}", requestedDisplaySizePx);
		}
		else
		{
			int xPos, yPos, width, height;
			glfwGetMonitorWorkarea(primaryMonitor, &xPos, &yPos, &width, &height);

			requestedDisplaySizePx.x() = width;
			requestedDisplaySizePx.y() = height;

			// HACK: scale it for now, often too large
			requestedDisplaySizePx /= 2;

			PH_LOG(GlfwPlatform, Note,
				"display size defaults to primary monitor work area {}", requestedDisplaySizePx);
		}
	}

	m_display.initialize(
		getEditor(), 
		settings.title, 
		requestedDisplaySizePx, 
		requestedGraphicsAPI,
		settings.useDebugModeGHI);

	m_input.initialize(getEditor(), m_display.getGlfwWindow());

	if(!m_display.getGraphicsContext())
	{
		PH_LOG(GlfwPlatform, Error,
			"no graphics context present, editor will likely crash");
	}
}

void GlfwPlatform::terminate()
{
	if(isGlfwWindowInitialized())
	{
		m_input.terminate();
		m_display.terminate();

		PH_LOG(GlfwPlatform, Note,
			"GLFW input stopped and window closed");
	}

	glfwTerminate();
}

}// end namespae ph::editor
