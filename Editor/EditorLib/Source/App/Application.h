#pragma once

#include "App/AppSettings.h"
#include "App/Editor.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <string>
#include <memory>

namespace ph::editor
{

class Platform;

class Application final
{
public:
	// TODO: CommandLineArguments for override or core settings?
	explicit Application(AppSettings settings);

	void run();

private:
	void close();

	AppSettings               m_settings;
	Editor                    m_editor;
	std::unique_ptr<Platform> m_platform;
};

}// end namespace ph::editor
