#include "App/Application.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"

#include <utility>

namespace ph::editor
{

Application::Application(AppSettings settings)
	: m_settings(std::move(settings))
	, m_editor()
	, m_platform()
{
	// TODO: threads

	m_platform = std::make_unique<GlfwPlatform>(m_settings, m_editor);
}

void Application::run()
{
	// TODO: update & render loop
}

void Application::close()
{
	// TODO
}

}// end namespace ph::editor
