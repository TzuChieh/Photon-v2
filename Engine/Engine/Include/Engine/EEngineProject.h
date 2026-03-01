#pragma once

#include <string_view>

namespace ph
{

/*! @brief Identifies a project within the Photon-v2 engine.
This is used to distinguish different components or sub-projects, such as the core engine, editor,
or command-line interface, often for project-specific settings and resource paths.
*/
enum class EEngineProject
{
	/*! @brief The core rendering engine. */
	Engine,

	/*! @brief The test suite for the core engine. */
	EngineTest,

	/*! @brief The deep test suite for the core engine. */
	EngineDeepTest,

	/*! @brief The Photon Scene Description Language (PSDL) generator. */
	SDLGen,

	/*! @brief The command-line interface for the PSDL generator. */
	SDLGenCLI,

	/*! @brief The command-line interface for the renderer. */
	PhotonCLI,

	/*! @brief The graphical editor for scene creation and rendering. */
	Editor,

	/*! @brief The core library for the editor. */
	EditorLib,

	/*! @brief The test suite for the editor library. */
	EditorTest,

	/*! @brief A tool for analyzing ray-primitive intersection errors. */
	IntersectError,

	/*! @brief Python bindings for the PSDL. */
	SDLPyBind,
};

/*! @brief Convert `EEngineProject` to string.
@return A string representation of the project name.
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
