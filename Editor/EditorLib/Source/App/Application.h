#pragma once

#include "App/Editor.h"
#include "Render/RenderThread.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <string>
#include <memory>
#include <vector>

namespace ph { class Path; }

namespace ph::editor
{

class AppSettings;
class Platform;
class ProcedureModule;
class RenderModule;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class AppModule;

class Application final
{
public:
	Application(int argc, char* argv[]);
	~Application();

	/*! @brief Run the application.
	This call will block until the application is going to be closed.
	*/
	void run();

	/*! @brief Close the application.
	Closing an already-closed application has no effect.
	*/
	void close();

	/*!
	Modules can only be attached before the application starts running, and detached only after the
	application finishes running.
	*/
	///@{
	bool attachProcedureModule(ProcedureModule* inModule);
	bool attachRenderModule(RenderModule* inModule);
	bool detachProcedureModule(ProcedureModule* inModule);
	bool detachRenderModule(RenderModule* inModule);
	///@}

private:
	void initialRenderThreadUpdate();
	void finalRenderThreadUpdate();
	void runMainLoop();
	void appStart();
	void appUpdate(const MainThreadUpdateContext& ctx);
	void appRenderUpdate(const MainThreadRenderUpdateContext& ctx);
	void appCreateRenderCommands();
	void appBeforeUpdateStage();
	void appAfterUpdateStage();
	void appBeforeRenderStage();
	void appAfterRenderStage();
	void appStop();
	bool attachModule(AppModule* targetModule);
	bool detachModule(AppModule* targetModule);
	/*void postModuleAttachedEvent(AppModule* targetModule);
	void postModuleDetachedEvent(AppModule* targetModule);*/

	static Path getCoreSettingsFile();

	std::shared_ptr<AppSettings>  m_settings;
	Editor                        m_editor;
	RenderThread                  m_renderThread;
	std::unique_ptr<Platform>     m_platform;
	std::vector<ProcedureModule*> m_procedureModules;
	std::vector<RenderModule*>    m_renderModules;
	std::vector<AppModule*>       m_modules;
	bool                          m_isRunning;
	bool                          m_shouldBreakMainLoop;
	bool                          m_isClosing;
};

}// end namespace ph::editor
