#include "SDLPyBind/library_bindings.h"

#include <Engine/ph_core.h>

#include <nanobind/nanobind.h>

namespace ph::py
{

NB_MODULE(PH_PY_BIND_MODULE_NAME, m)
{
	// Initialize the render engine upon import, before binding any of the APIs,
	// as the binding code may run some engine code.
	init_render_engine();

	// Hook render engine's exit function to Python's atexit module.
	// This will be called when the Python interpreter is exiting.
	nanobind::module_ atexit = nanobind::module_::import_("atexit");
	atexit.attr("register")(nanobind::cpp_function(
		[]()
		{
			exit_render_engine();
		}));

	bind_engine_sdl_definitions(m);
}

}// end namespace ph::py
