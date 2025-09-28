#pragma once

#include <string_view>

namespace ph
{

enum class EEngineProject
{
	Engine,
	EngineTest,
	EngineDeepTest,
	SDLGen,
	SDLGenCLI,
	PhotonCLI,
	Editor,
	EditorLib,
	EditorTest,
	IntersectError,
};

std::string_view to_string(EEngineProject project);

}// end namespace ph
