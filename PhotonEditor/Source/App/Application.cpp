#include "App/Application.h"

#include <utility>

namespace ph::editor
{

Application::Application(AppSettings settings) :
	m_settings(std::move(settings))
{}

void Application::run()
{
	// TODO
}

void Application::close()
{
	// TODO
}

}// end namespace ph::editor
