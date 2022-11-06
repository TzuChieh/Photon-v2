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

class Application final
{
public:
	// TODO: CommandLineArguments for override or core settings?
	explicit Application(AppSettings settings);
	~Application();

	/*! @brief Run the application.
	This call will block until the application is closed.
	*/
	void run();

private:
	/*! @brief Close the application.
	Closing an already-closed application has no effect.
	*/
	void close();

	void initialRenderThreadUpdate();
	void appMainLoop();
	void appUpdate(const MainThreadUpdateContext& ctx);
	void appRenderUpdate(const MainThreadRenderUpdateContext& ctx);
	uint64 getFrameTimeInUs() const;

	AppSettings                                   m_settings;
	Editor                                        m_editor;
	RenderThread                                  m_renderThread;
	std::unique_ptr<Platform>                     m_platform;
	std::vector<std::unique_ptr<ProcedureModule>> m_procedureModules;
	std::vector<std::unique_ptr<RenderModule>>    m_renderModules;
	bool                                          m_shouldClose;
};

}// end namespace ph::editor
