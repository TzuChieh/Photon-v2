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
	SDLPyBind,
};

/*! @brief Convert `EEngineProject` to string.
*/
inline constexpr std::string_view to_string(const EEngineProject project)
{
	switch(project)
	{
	case EEngineProject::Engine:         return "Engine";
	case EEngineProject::EngineTest:     return "EngineTest";
	case EEngineProject::EngineDeepTest: return "EngineDeepTest";
	case EEngineProject::SDLGen:         return "SDLGen";
	case EEngineProject::SDLGenCLI:      return "SDLGenCLI";
	case EEngineProject::PhotonCLI:      return "PhotonCLI";
	case EEngineProject::Editor:         return "Editor";
	case EEngineProject::EditorLib:      return "EditorLib";
	case EEngineProject::EditorTest:     return "EditorTest";
	case EEngineProject::IntersectError: return "IntersectError";
	case EEngineProject::SDLPyBind:      return "SDLPyBind";
	}

	return "";
}

}// end namespace ph
