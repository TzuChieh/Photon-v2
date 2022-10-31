#include "App/Application.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/RenderModule.h"

#include <utility>

namespace ph::editor
{

Application::Application(AppSettings settings)
	: m_settings(std::move(settings))
	, m_editor()
	, m_renderThread()
	, m_platform()
	, m_renderModules()
	, m_frameNumber(0)
	, m_isClosing(false)
{
	m_platform = std::make_unique<GlfwPlatform>(m_settings, m_editor);
	
	Threads::setRenderThreadID(m_renderThread.getWorkerThreadId());
	m_renderThread.startWorker();

	initialRenderThreadUpdate();
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

	// Wait for render thread to actually stop
	m_renderThread.waitForWorkerToStop();
	Threads::setRenderThreadID(std::thread::id());

	m_platform = nullptr;
}

void Application::initialRenderThreadUpdate()
{
	m_renderThread.beginFrame();

	m_renderThread.addGHIUpdateWork(m_platform->getDisplay().getGHI());

	m_renderThread.endFrame();
}

}// end namespace ph::editor
