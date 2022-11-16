#pragma once

#include <string_view>

namespace ph
{

enum class EEngineProject
{
	Engine,
	EngineTest,
	SDLGen,
	SDLGenCLI,
	PhotonCLI,
	Editor,
	EditorLib,
	EditorTest
};

std::string_view to_string(EEngineProject project);

}// end namespace ph
