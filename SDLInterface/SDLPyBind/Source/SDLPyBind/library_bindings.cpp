#include "library_bindings.h"

#include <Common/assertion.h>
#include <Engine/ph_core.h>
#include <Engine/SDL/Introspect/SdlClass.h>

namespace ph::py
{

namespace
{

inline bool is_python_binding_requested(const SdlClass* clazz)
{
	return clazz && clazz->getUserSpec().hasArg("interface", "python");
}

}// end namespace

void bind_engine_sdl_definitions(nanobind::module_& _)
{
	nanobind::module_& m = _.def_submodule("engine", "Core render engine.");

	for(const SdlClass* clazz : get_registered_engine_classes())
	{
		if(!is_python_binding_requested(clazz))
		{
			continue;
		}

		nanobind::class_<>

		// TODO
	}
}

}// end namespace ph::py
