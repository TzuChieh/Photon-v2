#include "App/Application.h"
#include "App/AppSettings.h"
#include "ph_editor.h"
#include "Platform/GlfwPlatform/GlfwPlatform.h"
#include "EditorCore/Thread/Threads.h"
#include "App/Module/ProcedureModule.h"
#include "App/Module/RenderModule.h"
#include "App/Module/MainThreadUpdateContext.h"
#include "App/Module/MainThreadRenderUpdateContext.h"
#include "App/Module/ModuleAttachmentInfo.h"
#include "Render/RenderThreadCaller.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/profiling.h>
#include <Utility/Timer.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/FileSystem/Filesystem.h>
#include <SDL/TSdl.h>

#include <utility>
#include <chrono>
#include <cstddef>
#include <thread>
#include <condition_variable>
#include <mutex>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Application, App);
PH_DEFINE_PROFILE_UNIT_NAME(MainLoop);

Application::Application(int argc, char* argv[])
	: m_settings(nullptr)
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
	// Load saved app settings. Create new one if not already present.
	{
		Path settingsFile = getCoreSettingsFile();
		if(Filesystem::hasFile(settingsFile))
		{
			m_settings = TSdl<AppSettings>::loadResource(settingsFile);
		}
		else
		{
			m_settings = TSdl<AppSettings>::makeResource();
		}
	}

	m_platform = std::make_unique<GlfwPlatform>(*m_settings, m_editor);
	
	m_editor.onDisplayClosed.addListener(
		[this](const DisplayClosedEvent& /* e */)
		{
			m_shouldBreakMainLoop = true;
		});

	m_editor.dimensionHints.applyDpiScale(m_platform->getDisplay().getDpiScale());

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

	// Save app settings.
	if(m_settings)
	{
		TSdl<AppSettings>::saveResource(m_settings, getCoreSettingsFile());
	}
}

void Application::run()
{
	// Set before starting the thread to make its memory effect visible on render thread.
	Threads::setRenderThread(&m_renderThread);

	m_renderThread.setGraphicsContext(m_platform->getDisplay().getGraphicsContext());
	m_renderThread.startWorker();

	initialRenderThreadUpdate();

	m_isRunning = true;
	runMainLoop();
	m_isRunning = false;

	finalRenderThreadUpdate();

	m_editor.cleanup();

	// Wait for render thread to actually stop
	m_renderThread.waitForWorkerToStop();
	Threads::setRenderThread(nullptr);
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
	PH_PROFILE_SCOPE();

	m_renderThread.beginFrame();

	for(RenderModule* const renderModule : m_renderModules)
	{
		RenderThreadCaller caller(m_renderThread);
		renderModule->createSetupRenderCommands(caller);
	}

	m_renderThread.endFrame();
}

void Application::finalRenderThreadUpdate()
{
	PH_PROFILE_SCOPE();

	m_renderThread.beginFrame();

	for(RenderModule* const renderModule : m_renderModules)
	{
		RenderThreadCaller caller(m_renderThread);
		renderModule->createCleanupRenderCommands(caller);
	}

	{
		RenderThreadCaller caller(m_renderThread);
		m_editor.renderCleanup(caller);
	}

	m_renderThread.requestWorkerStop();
	m_renderThread.endFrame();
}

void Application::runMainLoop()
{
	appStart();

	std::mutex loopMutex;
	std::condition_variable loopCv;

	MainThreadUpdateContext updateCtx;
	MainThreadRenderUpdateContext renderUpdateCtx;

	using TimeUnit = std::chrono::nanoseconds;

	PH_ASSERT_NE(m_settings->maxFPS, 0);
	const auto frameTime = TimeUnit(std::chrono::seconds(1)) / m_settings->maxFPS;

	auto unprocessedTime = TimeUnit::zero();

	Timer loopTimer;
	loopTimer.start();

	while(!m_shouldBreakMainLoop)
	{
		const auto passedTime = TimeUnit(loopTimer.markLap());

		m_editor.editorStats.mainThreadFrameMs = std::chrono::duration<float32, std::milli>(passedTime).count();

		bool hasUpdated = false;
		bool hasRendered = false;
		unprocessedTime += passedTime;

		// Update
		while(unprocessedTime >= frameTime)
		{
			// Call only once in each main loop
			if(!hasUpdated)
			{
				appBeforeUpdateStage();
			}

			auto updateTimer = Timer().start();

			updateCtx.deltaS = std::chrono::duration<float64>(frameTime).count();
			appUpdate(updateCtx);

			++updateCtx.frameNumber;
			unprocessedTime -= frameTime;
			hasUpdated = true;

			m_editor.editorStats.mainThreadUpdateMs = updateTimer.stop().getDeltaMs<float32>();
		}// If there's still bunch of unprocessed time, update the editor again

		if(hasUpdated)
		{
			appAfterUpdateStage();
		}

		// Render update
		if(hasUpdated)
		{
			// Call only once in each main loop
			if(!hasRendered)
			{
				appBeforeRenderStage();
			}

			m_renderThread.beginFrame();

			auto renderTimer = Timer().start();

			const auto renderFrameInfo = m_renderThread.getFrameInfo();
			renderUpdateCtx.frameNumber = renderFrameInfo.frameNumber;
			renderUpdateCtx.frameCycleIndex = renderFrameInfo.frameCycleIndex;
			appRenderUpdate(renderUpdateCtx);
			appCreateRenderCommands();

			hasRendered = true;
			m_editor.editorStats.mainThreadRenderMs = renderTimer.stop().getDeltaMs<float32>();

			m_renderThread.endFrame();
		}

		m_editor.editorStats.renderThreadFrameMs = m_renderThread.getFrameTimeMs();
		m_editor.editorStats.ghiThreadFrameMs = m_renderThread.getGHIFrameTimeMs();

		if(hasRendered)
		{
			appAfterRenderStage();
		}

		// Wait for next update
		{
			const auto currentLapTime = TimeUnit(loopTimer.peekLap());
			const auto currentUnprocessedTime = unprocessedTime + currentLapTime;

			// Already behind more than a frame, just yield
			if(currentUnprocessedTime >= frameTime)
			{
				std::this_thread::yield();
			}
			// Otherwise, sleep until a frame of unprocessed time is accumulated
			else
			{
				const auto timeTillNextFrame = frameTime - currentUnprocessedTime;

				std::unique_lock<std::mutex> loopLock(loopMutex);

				// The thread may unwait spuriously, in such case the current unprocessed
				// time is checked again
				loopCv.wait_for(
					loopLock, 
					timeTillNextFrame,
					[&loopTimer, &unprocessedTime, &frameTime]()
					{
						const auto currentLapTime = TimeUnit(loopTimer.peekLap());
						const auto currentUnprocessedTime = unprocessedTime + currentLapTime;
						return currentUnprocessedTime >= frameTime;
					});
			}
		}

		PH_PROFILE_LOOP_MARK(MainLoop);
	}// end while `!m_shouldBreakMainLoop`

	appStop();
}

void Application::appStart()
{
	PH_PROFILE_SCOPE();

	m_editor.start();
}

void Application::appUpdate(const MainThreadUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	m_editor.update(ctx);

	for(auto& procedureModule : m_procedureModules)
	{
		procedureModule->update(ctx);
	}

	m_platform->update(ctx.deltaS);
}

void Application::appRenderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	m_editor.renderUpdate(ctx);

	for(auto& renderModule : m_renderModules)
	{
		renderModule->renderUpdate(ctx);
	}
}

void Application::appCreateRenderCommands()
{
	PH_PROFILE_SCOPE();

	{
		RenderThreadCaller caller(m_renderThread);
		m_editor.createRenderCommands(caller);
	}

	for(auto& renderModule : m_renderModules)
	{
		RenderThreadCaller caller(m_renderThread);
		renderModule->createRenderCommands(caller);
	}
}

void Application::appBeforeUpdateStage()
{
	PH_PROFILE_SCOPE();

	m_editor.beforeUpdateStage();
}

void Application::appAfterUpdateStage()
{
	PH_PROFILE_SCOPE();

	m_editor.afterUpdateStage();
}

void Application::appBeforeRenderStage()
{
	PH_PROFILE_SCOPE();

	m_editor.beforeRenderStage();
}

void Application::appAfterRenderStage()
{
	PH_PROFILE_SCOPE();

	m_editor.afterRenderStage();
}

void Application::appStop()
{
	PH_PROFILE_SCOPE();

	m_editor.stop();
}

bool Application::attachProcedureModule(ProcedureModule* const inModule)
{
	PH_PROFILE_SCOPE();

	if(!attachModule(inModule))
	{
		return false;
	}

	m_procedureModules.push_back(inModule);
	return true;
}

bool Application::attachRenderModule(RenderModule* const inModule)
{
	PH_PROFILE_SCOPE();

	if(!attachModule(inModule))
	{
		return false;
	}

	m_renderModules.push_back(inModule);
	return true;
}

bool Application::detachProcedureModule(ProcedureModule* const inModule)
{
	PH_PROFILE_SCOPE();

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
	PH_PROFILE_SCOPE();

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
	PH_PROFILE_SCOPE();

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
	PH_PROFILE_SCOPE();

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

	if(const auto numErased = std::erase(m_modules, targetModule); numErased != 1)
	{
		PH_LOG_WARNING(Application,
			"erased {} {} modules, expecting exactly 1 module", 
			numErased, targetModule->getName());
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

Path Application::getCoreSettingsFile()
{
	return get_editor_data_directory() / "AppCoreSettings.p2";
}

}// end namespace ph::editor
