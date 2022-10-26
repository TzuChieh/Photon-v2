#pragma once

#include "App/AppSettings.h"
#include "App/Editor.h"
#include "EditorCore/Thread/RenderThread.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <string>
#include <memory>
#include <vector>
#include <cstddef>

namespace ph::editor
{

class Platform;
class RenderModule;

class Application final
{
public:
	// TODO: CommandLineArguments for override or core settings?
	explicit Application(AppSettings settings);
	~Application();

	void run();

private:
	/*! @brief Close the application.
	Closing an already-closed application has no effect.
	*/
	void close();

	AppSettings                                m_settings;
	Editor                                     m_editor;
	RenderThread                               m_renderThread;
	std::unique_ptr<Platform>                  m_platform;
	std::vector<std::unique_ptr<RenderModule>> m_renderModules;
	std::size_t                                m_frameNumber;
	bool                                       m_isClosing;
};

}// end namespace ph::editor
