#include "App/Application.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "EditorCore/Thread/Threads.h"
#include "App/Module/ProcedureModule.h"
#include "App/Module/RenderModule.h"
#include "App/Module/MainThreadUpdateContext.h"
#include "App/Module/MainThreadRenderUpdateContext.h"
#include "App/Module/ModuleAttachmentInfo.h"

#include <Common/assertion.h>
#include <Utility/Timer.h>
#include <Common/logging.h>

#include <utility>
#include <chrono>
#include <cstddef>
#include <thread>
#include <condition_variable>
#include <mutex>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Application, App);

Application::Application(AppSettings settings)
	: m_settings(std::move(settings))
	, m_editor()
	, m_renderThread()
	, m_platform()
	, m_procedureModules()
	, m_renderModules()
	, m_isRunning(false)
	, m_shouldBreakMainLoop(false)
	, m_isClosing(false)
{
	m_platform = std::make_unique<GlfwPlatform>(m_settings, m_editor);
	
	m_editor.onDisplayClose.addListener(
		[this](const DisplayCloseEvent& /* e */)
		{
			m_shouldBreakMainLoop = true;
		});

	m_editor.onAppModuleAction.addListener(
		[this](const AppModuleActionEvent& e)
		{
			if(e.getAction() == EAppModuleAction::Attach)
			{
				ModuleAttachmentInfo info;
				info.editor = &m_editor;
				info.frameBufferSizePx = m_platform->getDisplay().getFrameBufferSizePx();

				e.getTargetModule()->onAttach(info);
			}
			else if(e.getAction() == EAppModuleAction::Detach)
			{
				e.getTargetModule()->onDetach();
			}
		});

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
	m_isRunning = true;

	appMainLoop();
	close();
}

void Application::close()
{
	if(m_isClosing)
	{
		return;
	}

	m_isRunning = false;
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

void Application::appMainLoop()
{
	std::mutex loopMutex;
	std::condition_variable loopCv;

	MainThreadUpdateContext updateCtx;
	MainThreadRenderUpdateContext renderUpdateCtx;

	using TimeUnit = std::chrono::nanoseconds;

	constexpr auto numNsTicksFor1S = std::chrono::duration_cast<TimeUnit>(std::chrono::seconds(1));

	PH_ASSERT_NE(m_settings.maxFPS, 0);
	const auto frameTime  = numNsTicksFor1S / m_settings.maxFPS;
	const auto frameTimeS = static_cast<float64>(frameTime.count()) / static_cast<float64>(numNsTicksFor1S.count());

	auto unprocessedTime = TimeUnit::zero();

	Timer loopTimer;
	loopTimer.start();

	while(!m_shouldBreakMainLoop)
	{
		const auto passedTime = std::chrono::duration_cast<TimeUnit>(loopTimer.markLap());

		bool shouldRender = false;
		unprocessedTime += passedTime;

		// Update
		while(unprocessedTime > frameTime)
		{
			updateCtx.deltaS = frameTimeS;

			appUpdate(updateCtx);

			++updateCtx.frameNumber;
			unprocessedTime -= frameTime;
			shouldRender = true;
		}// If there's still bunch of unprocessed time, update the editor again

		// Render update
		if(shouldRender)
		{
			m_renderThread.beginFrame();

			const auto renderFrameInfo = m_renderThread.getFrameInfo();
			renderUpdateCtx.frameNumber = renderFrameInfo.frameNumber;
			renderUpdateCtx.frameCycleIndex = renderFrameInfo.frameCycleIndex;

			appRenderUpdate(renderUpdateCtx);

			m_renderThread.endFrame();
		}

		// Wait for next update
		{
			const auto currentLapTime = std::chrono::duration_cast<TimeUnit>(loopTimer.peekLap());
			const auto currentUnprocessedTime = unprocessedTime + currentLapTime;

			// Already behind more than a frame, just yield
			if(currentUnprocessedTime >= frameTime)
			{
				std::this_thread::yield();
			}
			// Otherwise, sleep until a frame of unprocessed time is available
			else
			{
				const auto timeTillNextFrame = frameTime - currentUnprocessedTime;

				std::unique_lock<std::mutex> loopLock(loopMutex);
				loopCv.wait_for(loopLock, timeTillNextFrame);// TODO: check return type and possibly sleep more
			}
		}
	}// end while `!m_shouldClose`
}

void Application::appUpdate(const MainThreadUpdateContext& ctx)
{
	m_editor.flushAllEvents();

	// TODO: editor scene update
	

	for(auto& procedureModule : m_procedureModules)
	{
		procedureModule->update(ctx);
	}

	m_platform->update(ctx.deltaS);
}

void Application::appRenderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	for(auto& renderModule : m_renderModules)
	{
		renderModule->renderUpdate(ctx);
	}
}

void Application::attachProcedureModule(ProcedureModule* const inModule)
{
	if(!validateStatusForModuleAction(inModule, EAppModuleAction::Attach))
	{
		return;
	}

	m_procedureModules.push_back(inModule);
	postModuleAttachedEvent(inModule);
}

void Application::attachRenderModule(RenderModule* const inModule)
{
	if(!validateStatusForModuleAction(inModule, EAppModuleAction::Attach))
	{
		return;
	}

	m_renderModules.push_back(inModule);
	postModuleAttachedEvent(inModule);
}

void Application::detachProcedureModule(ProcedureModule* const inModule)
{
	if(!validateStatusForModuleAction(inModule, EAppModuleAction::Detach))
	{
		return;
	}

	const auto numErasedModules = std::erase(m_procedureModules, inModule);
	if(numErasedModules == 0)
	{
		PH_LOG_WARNING(Application, 
			"unable to detach procedure module {} since it was not attached",
			inModule->getName());
		return;
	}

	postModuleDetachedEvent(inModule);
}

void Application::detachRenderModule(RenderModule* const inModule)
{
	if(!validateStatusForModuleAction(inModule, EAppModuleAction::Detach))
	{
		return;
	}

	const auto numErasedModules = std::erase(m_renderModules, inModule);
	if(numErasedModules == 0)
	{
		PH_LOG_WARNING(Application,
			"unable to detach render module {} since it was not attached",
			inModule->getName());
		return;
	}

	postModuleDetachedEvent(inModule);
}

bool Application::validateStatusForModuleAction(AppModule* const targetModule, const EAppModuleAction intent)
{
	if(!targetModule)
	{
		PH_LOG_WARNING(Application, "unable perform actions on an empty module");
		return false;
	}

	if(intent == EAppModuleAction::Attach)
	{
		if(m_isRunning || m_isClosing)
		{
			PH_LOG_WARNING(Application, 
				"cannot attach {} module while the app is running or closing", 
				targetModule->getName());
			return false;
		}
	}
	else if(intent == EAppModuleAction::Detach)
	{
		if(m_isRunning)
		{
			PH_LOG_WARNING(Application,
				"cannot detech {} module while the app is running",
				targetModule->getName());
			return false;
		}
	}

	return true;
}

void Application::postModuleAttachedEvent(AppModule* const targetModule)
{
	PH_ASSERT(targetModule);

	m_editor.postEvent(
		AppModuleActionEvent(EAppModuleAction::Attach, targetModule),
		m_editor.onAppModuleAction);

	PH_LOG(Application,
		"{} module attached",
		targetModule->getName());
}

void Application::postModuleDetachedEvent(AppModule* const targetModule)
{
	PH_ASSERT(targetModule);

	m_editor.postEvent(
		AppModuleActionEvent(EAppModuleAction::Detach, targetModule),
		m_editor.onAppModuleAction);

	PH_LOG(Application,
		"{} module detached",
		targetModule->getName());
}

}// end namespace ph::editor
