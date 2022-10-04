#pragma once

#include "App/AppSettings.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <string>

namespace ph::editor
{

class Application final
{
public:
	// TODO: CommandLineArguments for override or core settings?
	explicit Application(AppSettings settings);

	void run();

private:
	void close();

	AppSettings m_settings;
};

}// end namespace ph::editor
