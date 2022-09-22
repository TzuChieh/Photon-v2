#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "App/AppSettings.h"

#include <Common/logging.h>
#include <Common/assertion.h>

#include <GLFW/glfw3.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(GlfwPlatform, EditorPlatform);

namespace
{

inline void glfw_error_callback(const int errorCode, const char* const errorDescription)
{
	PH_LOG_ERROR(GlfwPlatform, "GLFW error: {} (error code = {})",
		errorDescription, errorCode);
}

}// end anonymous namespace

GlfwPlatform::GlfwPlatform(const AppSettings& settings)
	: Platform()
	, m_glfwWindow(nullptr)
	, m_input()
	, m_display()
{
	init(settings);
}

GlfwPlatform::~GlfwPlatform()
{
	terminate();
}

void GlfwPlatform::update(const float64 deltaS)
{
	// TODO
}

void GlfwPlatform::render()
{
	// TODO
}

bool GlfwPlatform::isInitialized() const
{
	return isGlfwWindowInitialized();
}

void GlfwPlatform::init(const AppSettings& settings)
{
	glfwSetErrorCallback(glfw_error_callback);

	const int initReturnCode = glfwInit();
	if(initReturnCode != GLFW_TRUE)
	{
		throw PlatformException(
			"error occurred on initializing GLFW");
	}

	EGraphicsAPI requestedGraphicsApi = settings.graphicsApi;
	if(settings.graphicsApi == EGraphicsAPI::Default)
	{
		PH_LOG(GlfwPlatform, "graphics API defaults to OpenGL");

		requestedGraphicsApi = EGraphicsAPI::OpenGL;
	}

	m_display.createWindow(settings.title, settings.displaySizePx, requestedGraphicsApi);

	// TODO
}

void GlfwPlatform::terminate()
{
	if(isGlfwWindowInitialized())
	{
		m_input.stop();
		m_display.closeWindow();

		PH_LOG(GlfwPlatform, "GLFW input stopped and window closed");
	}

	glfwTerminate();
}

}// end namespae ph::editor
