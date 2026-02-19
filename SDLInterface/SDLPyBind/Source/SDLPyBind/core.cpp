#include "SDLPyBind/library_bindings.h"

#include <Engine/ph_core.h>
#include <Engine/EngineInitSettings.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Common/exceptions.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <string>

namespace ph::py
{

NB_MODULE(PH_PY_BIND_MODULE_NAME, m)
{
	std::string pydPath = nanobind::cast<std::string>(m.attr("__file__"));

	EngineInitSettings initSettings = EngineInitSettings::loadStandardConfig(pydPath);
	initSettings.installationDirectory = Filesystem::findInstallationDirectory(pydPath).toString();

	// Initialize the render engine upon import, before binding any of the APIs,
	// as the binding code may run some engine code.
	if(!init_render_engine(initSettings))
	{
		throw RuntimeException("Engine initialization failed.");
	}

	// Hook render engine's exit function to Python's atexit module.
	// This will be called when the Python interpreter is exiting.
	nanobind::module_ atexit = nanobind::module_::import_("atexit");
	atexit.attr("register")(nanobind::cpp_function(
		[]()
		{
			if(!exit_render_engine())
			{
				throw RuntimeException("Engine clean up failed.");
			}
		}));

	bind_engine_sdl_definitions(m);
}

}// end namespace ph::py
