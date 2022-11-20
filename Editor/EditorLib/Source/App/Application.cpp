#include "App/Application.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "EditorCore/Thread/Threads.h"
#include "App/Module/ProcedureModule.h"
#include "App/Module/RenderModule.h"
#include "App/Module/MainThreadUpdateContext.h"
#include "App/Module/MainThreadRenderUpdateContext.h"
#include "App/Module/ModuleAttachmentInfo.h"
#include "EditorCore/Thread/RenderThreadCaller.h"

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
	, m_modules()
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

	/*m_editor.onAppModuleAction.addListener(
		[this](const AppModuleActionEvent& e)
		{
			if(e.getAction() == EAppModuleAction::Attach)
			{
				ModuleAttachmentInfo info;
				info.platform          = m_platform.get();
				info.editor            = &m_editor;
				info.framebufferSizePx = m_platform->getDisplay().getFramebufferSizePx();

				e.getTargetModule()->onAttach(info);
			}
			else if(e.getAction() == EAppModuleAction::Detach)
			{
				e.getTargetModule()->onDetach();
			}
		});*/
}

Application::~Application()
{
	close();
}

void Application::run()
{
	// Thread ID set before starting the thread to make its memory effect visible on render thread.
	Threads::setRenderThreadID(m_renderThread.getWorkerThreadId());
	m_renderThread.startWorker();

	initialRenderThreadUpdate();

	m_isRunning = true;
	appMainLoop();
	m_isRunning = false;

	// Request to stop the render thread
	m_renderThread.beginFrame();
	m_renderThread.requestWorkerStop();
	m_renderThread.endFrame();

	// Wait for render thread to actually stop
	m_renderThread.waitForWorkerToStop();
	Threads::setRenderThreadID(std::thread::id());
}

void Application::close()
{
	if(m_isClosing)
	{
		return;
	}

	m_isClosing = true;

	if(!m_procedureModules.empty() || !m_renderModules.empty())
	{
		PH_LOG_WARNING(Application,
			"some modules are not detatched upon closing: "
			"{} procedure modules, "
			"{} render modules",
			m_procedureModules.size(),
			m_renderModules.size());
	}

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

			appCreateRenderCommands();

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

void Application::appCreateRenderCommands()
{
	for(auto& renderModule : m_renderModules)
	{
		RenderThreadCaller caller(m_renderThread);
		renderModule->createRenderCommands(caller);
	}
}

bool Application::attachProcedureModule(ProcedureModule* const inModule)
{
	if(!attachModule(inModule))
	{
		return false;
	}

	m_procedureModules.push_back(inModule);
	return true;
}

bool Application::attachRenderModule(RenderModule* const inModule)
{
	if(!attachModule(inModule))
	{
		return false;
	}

	m_renderModules.push_back(inModule);
	return true;
}

bool Application::detachProcedureModule(ProcedureModule* const inModule)
{
	const auto numErasedModules = std::erase(m_procedureModules, inModule);
	if(numErasedModules == 0)
	{
		PH_LOG_WARNING(Application,
			"unable to remove procedure module {} since it was not added",
			inModule->getName());
		return false;
	}

	return detachModule(inModule);
}

bool Application::detachRenderModule(RenderModule* const inModule)
{
	const auto numErasedModules = std::erase(m_renderModules, inModule);
	if(numErasedModules == 0)
	{
		PH_LOG_WARNING(Application,
			"unable to remove render module {} since it was not added",
			inModule->getName());
		return false;
	}

	return detachModule(inModule);
}

bool Application::attachModule(AppModule* const targetModule)
{
	if(!targetModule)
	{
		PH_LOG_WARNING(Application,
			"unable to attach an empty module");
		return false;
	}

	if(m_isRunning || m_isClosing)
	{
		PH_LOG_WARNING(Application,
			"cannot attach {} module while the app is running or closing",
			targetModule->getName());
		return false;
	}


	ModuleAttachmentInfo info;
	info.platform = m_platform.get();
	info.editor = &m_editor;
	info.framebufferSizePx = m_platform->getDisplay().getFramebufferSizePx();

	for(const AppModule* appModule : m_modules)
	{
		info.attachedModuleNames.push_back(appModule->getName());
	}

	try
	{
		targetModule->onAttach(info);
	}
	catch(const ModuleException& e)
	{
		PH_LOG_WARNING(Application,
			"cannot attach {} module: {}",
			targetModule->getName(), e.whatStr());
		return false;
	}

	m_modules.push_back(targetModule);

	PH_LOG(Application,
		"{} module attached",
		targetModule->getName());

	return true;
}

bool Application::detachModule(AppModule* const targetModule)
{
	if(!targetModule)
	{
		PH_LOG_WARNING(Application,
			"unable to detach an empty module");
		return false;
	}

	if(m_isRunning || m_isClosing)
	{
		PH_LOG_WARNING(Application,
			"cannot detech {} module while the app is running or closing",
			targetModule->getName());
		return false;
	}

	try
	{
		targetModule->onDetach();
	}
	catch(const ModuleException& e)
	{
		PH_LOG_WARNING(Application,
			"cannot detach {} module: {}",
			targetModule->getName(), e.whatStr());
		return false;
	}

	PH_LOG(Application,
		"{} module detached",
		targetModule->getName());

	return true;
}

//void Application::postModuleAttachedEvent(AppModule* const targetModule)
//{
//	PH_ASSERT(targetModule);
//
//	m_editor.postEvent(
//		AppModuleActionEvent(EAppModuleAction::Attach, targetModule),
//		m_editor.onAppModuleAction);
//
//	PH_LOG(Application,
//		"{} module attached",
//		targetModule->getName());
//}
//
//void Application::postModuleDetachedEvent(AppModule* const targetModule)
//{
//	PH_ASSERT(targetModule);
//
//	m_editor.postEvent(
//		AppModuleActionEvent(EAppModuleAction::Detach, targetModule),
//		m_editor.onAppModuleAction);
//
//	PH_LOG(Application,
//		"{} module detached",
//		targetModule->getName());
//}

}// end namespace ph::editor
