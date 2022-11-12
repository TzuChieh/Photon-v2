#pragma once

#include "App/AppSettings.h"
#include "App/Editor.h"
#include "EditorCore/Thread/RenderThread.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <string>
#include <memory>
#include <vector>

namespace ph::editor
{

class Platform;
class ProcedureModule;
class RenderModule;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class AppModule;

class Application final
{
public:
	// TODO: CommandLineArguments for override or core settings?
	explicit Application(AppSettings settings);
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
	void attachProcedureModule(ProcedureModule* inModule);
	void attachRenderModule(RenderModule* inModule);
	void detachProcedureModule(ProcedureModule* inModule);
	void detachRenderModule(RenderModule* inModule);
	///@}

private:
	void initialRenderThreadUpdate();
	void appMainLoop();
	void appUpdate(const MainThreadUpdateContext& ctx);
	void appRenderUpdate(const MainThreadRenderUpdateContext& ctx);
	void attachModule(AppModule* targetModule);
	void detachModule(AppModule* targetModule);
	/*void postModuleAttachedEvent(AppModule* targetModule);
	void postModuleDetachedEvent(AppModule* targetModule);*/

	AppSettings                   m_settings;
	Editor                        m_editor;
	RenderThread                  m_renderThread;
	std::unique_ptr<Platform>     m_platform;
	std::vector<ProcedureModule*> m_procedureModules;
	std::vector<RenderModule*>    m_renderModules;
	bool                          m_isRunning;
	bool                          m_shouldBreakMainLoop;
	bool                          m_isClosing;
};

}// end namespace ph::editor
