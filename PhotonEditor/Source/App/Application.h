#pragma once

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <string>

namespace ph::editor
{

// TODO: can be load/save from/to file, etc
class AppSettings final
{
public:
	std::string    title         = "Photon Editor";
	math::Vector2S displaySizePx = {0, 0};
	uint32f        maxFPS        = 60;
};

class Application final
{
public:
	// TODO: CommandLineArguments for override or core settings?
	explicit Application(AppSettings settings);

private:
	AppSettings m_settings;
};

}// end namespace ph::editor
