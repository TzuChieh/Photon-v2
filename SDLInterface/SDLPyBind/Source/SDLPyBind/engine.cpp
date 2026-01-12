#include <Engine/ph_core.h>

#include <nanobind/nanobind.h>

namespace ph::py
{

NB_MODULE(PH_PY_BIND_MODULE_NAME, m)
{
	m.def(
		"init_render_engine",
		[]()
		{
			return init_render_engine();
		},
		"Initialize the render engine.");

	m.def(
		"exit_render_engine",
		[]()
		{
			return exit_render_engine();
		},
		"Exit the render engine.");

	// TODO: submodule for generated SDL bindings
}

}// end namespace ph::py
