#include "EEngineProject.h"

#include <Common/assertion.h>

namespace ph
{

std::string_view to_string(const EEngineProject project)
{
	switch(project)
	{
	case EEngineProject::Engine:         return "Engine";
	case EEngineProject::EngineTest:     return "EngineTest";
	case EEngineProject::SDLGen:         return "SDLGen";
	case EEngineProject::SDLGenCLI:      return "SDLGenCLI";
	case EEngineProject::PhotonCLI:      return "PhotonCLI";
	case EEngineProject::Editor:         return "Editor";
	case EEngineProject::EditorLib:      return "EditorLib";
	case EEngineProject::EditorTest:     return "EditorTest";
	case EEngineProject::IntersectError: return "IntersectError";
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return "./";
}


}// end namespace ph
