#include "App/Application.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "EditorCore/Thread/Threads.h"
#include "Procedure/ProcedureModule.h"
#include "Render/RenderModule.h"
#include "App/MainThreadUpdateContext.h"
#include "Render/MainThreadRenderUpdateContext.h"

#include <Common/assertion.h>

#include <utility>
#include <chrono>
#include <cstddef>
#include <thread>

namespace ph::editor
{

Application::Application(AppSettings settings)
	: m_settings(std::move(settings))
	, m_editor()
	, m_renderThread()
	, m_platform()
	, m_procedureModules()
	, m_renderModules()
	, m_shouldClose(false)
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
	appMainLoop();
	close();
}

void Application::close()
{
	if(m_shouldClose)
	{
		return;
	}

	m_shouldClose = true;

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

void Application::appMainLoop()
{
	MainThreadUpdateContext updateCtx;
	MainThreadRenderUpdateContext renderUpdateCtx;

	const uint64 frameTimeUs = getFrameTimeInUs();
	const auto   frameTimeS  = static_cast<float64>(frameTimeUs) / (1000.0 * 1000.0);

	using Clock = std::chrono::steady_clock;
	Clock::time_point lastStartTime     = Clock::now();
	Clock::time_point startTime         = lastStartTime;
	uint64            unprocessedTimeUs = 0;

	while(!m_shouldClose)
	{
		startTime = Clock::now();
		const auto passedTimeUs = static_cast<uint64>(std::chrono::duration_cast<std::chrono::microseconds>(
			startTime - lastStartTime).count());
		lastStartTime = startTime;

		unprocessedTimeUs += passedTimeUs;

		// Update
		while(unprocessedTimeUs > frameTimeUs)
		{
			updateCtx.deltaS = frameTimeS;

			appUpdate(updateCtx);

			++updateCtx.frameNumber;
			unprocessedTimeUs -= frameTimeUs;
		}// If there's still bunch of unprocessed time, update the editor again

		// Render update
		{
			m_renderThread.beginFrame();

			const auto renderFrameInfo = m_renderThread.getFrameInfo();
			renderUpdateCtx.frameNumber = renderFrameInfo.frameNumber;
			renderUpdateCtx.frameCycleIndex = renderFrameInfo.frameCycleIndex;

			appRenderUpdate(renderUpdateCtx);

			m_renderThread.endFrame();
		}



		std::this_thread::yield();
	}
}

void Application::appUpdate(const MainThreadUpdateContext& ctx)
{
	// TODO
}

void Application::appRenderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	// TODO
}

uint64 Application::getFrameTimeInUs() const
{
	PH_ASSERT_NE(m_settings.maxFPS, 0);
	const double secondsPerFrame = 1.0 / m_settings.maxFPS;
	return static_cast<uint64>(secondsPerFrame * 1000.0 * 1000.0);
}

}// end namespace ph::editor
