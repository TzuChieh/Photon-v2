#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "App/AppSettings.h"
#include "ThirdParty/GLFW3.h"

#include <Common/logging.h>
#include <Common/assertion.h>

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

void GlfwPlatform::initialize(const AppSettings& settings)
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

	m_display.initialize(getEditor(), settings.title, settings.displaySizePx, requestedGraphicsApi);
	m_input.initialize(getEditor(), m_display.getGlfwWindow());

	// TODO: GHI related
}

void GlfwPlatform::terminate()
{
	if(isGlfwWindowInitialized())
	{
		m_input.terminate();
		m_display.terminate();

		PH_LOG(GlfwPlatform, "GLFW input stopped and window closed");
	}

	glfwTerminate();
}

}// end namespae ph::editor