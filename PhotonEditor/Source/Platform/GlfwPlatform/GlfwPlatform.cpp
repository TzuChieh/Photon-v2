#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "App/Application.h"

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
	
}

GlfwPlatform::~GlfwPlatform()
{
	terminate();
}

void GlfwPlatform::update(const float64 deltaS)
{

}

bool GlfwPlatform::isInitialized() const
{
	return isGlfwInitialized();
}

bool GlfwPlatform::init(const AppSettings& settings)
{
	glfwSetErrorCallback(glfw_error_callback);

	const int initReturnCode = glfwInit();
	if(initReturnCode == GLFW_TRUE)
	{
		PH_LOG(GlfwPlatform, "GLFW initialized");
	}
	else
	{
		PH_ASSERT_EQ(initReturnCode, GLFW_FALSE);
		PH_LOG_ERROR(GlfwPlatform, "error occurred on initializing GLFW");
		return;
	}


}

void GlfwPlatform::terminate()
{
	if(isGlfwInitialized())
	{
		m_display.destroyWindow();
		glfwTerminate();

		PH_LOG(GlfwPlatform, "GLFW terminated");
	}
}

}// end namespae ph::editor
