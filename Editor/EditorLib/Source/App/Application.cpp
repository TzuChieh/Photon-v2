#include "App/Application.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "EditorCore/Thread/Threads.h"

#include <utility>

namespace ph::editor
{

Application::Application(AppSettings settings)
	: m_settings(std::move(settings))
	, m_editor()
	, m_platform()
	, m_renderModules()
	, m_renderThread()
	, m_frameNumber(0)
	, m_isClosing(false)
{
	Threads::setRenderThread(&m_renderThread);

	// TODO: threads

	m_platform = std::make_unique<GlfwPlatform>(m_settings, m_editor);
}

Application::~Application()
{
	close();
}

void Application::run()
{
	// TODO: update & render loop
}

void Application::close()
{
	if(m_isClosing)
	{
		return;
	}

	m_isClosing = true;

	// Request to stop the render thread
	m_renderThread.beginFrame();
	m_renderThread.requestWorkerStop();
	m_renderThread.endFrame();

	// TODO: stop GHI thread

	// Wait for all thread to actually stop
	m_renderThread.waitForWorkerToStop();
	// TODO: wait for GHI thread to stop
}

}// end namespace ph::editor
