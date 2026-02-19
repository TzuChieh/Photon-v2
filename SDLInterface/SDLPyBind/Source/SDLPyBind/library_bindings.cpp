#include "library_bindings.h"

#include <Common/assertion.h>
#include <Engine/ph_core.h>

namespace ph::py
{

void bind_engine_sdl_definitions(nanobind::module_ _)
{
	nanobind::module_ m = _.def_submodule("engine", "Core render engine.");

	UniversalSDLBinder binder(m);
	bind_engine(binder);
}

std::string UniversalSDLBinder::toSdlTypeName(nanobind::handle pyValue)
{
	std::string sdlType;
	if(nanobind::isinstance<nanobind::bool_>(pyValue))
	{
		sdlType = "bool";
	}
	else if(nanobind::isinstance<nanobind::int_>(pyValue))
	{
		sdlType = "integer";
	}
	else if(nanobind::isinstance<nanobind::float_>(pyValue))
	{
		sdlType = "real";
	}
	else if(nanobind::isinstance<nanobind::str>(pyValue))
	{
		sdlType = "string";
	}
	else
	{
		const std::string msg = 
			"Unable to map Python value type <" + 
			std::string(nanobind::inst_name(pyValue).c_str()) + 
			"> to SDL.";
		throw nanobind::type_error(msg.c_str());
	}

	return sdlType;
}

SdlInputClauses UniversalSDLBinder::toSdlInputClauses(nanobind::kwargs kwargs)
{
	SdlInputClauses clauses;
	for(auto [key, value] : kwargs)
	{
		clauses.add(
			SdlInputClause(
				toSdlTypeName(value),
				nanobind::cast<std::string>(nanobind::str(key)),
				nanobind::cast<std::string>(nanobind::str(value))));
	}

	return clauses;
}

}// end namespace ph::py
