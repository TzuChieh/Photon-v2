#include "App/Application.h"

#include <utility>

namespace ph::editor
{

Application::Application(AppSettings settings) :
	m_settings(std::move(settings))
{}

}// end namespace ph::editor
