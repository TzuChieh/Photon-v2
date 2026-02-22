#include "library_bindings.h"

#include <Common/assertion.h>
#include <Engine/ph_core.h>

#include <unordered_map>

namespace ph::py
{

void bind_engine_sdl_definitions(nanobind::module_ _)
{
	nanobind::module_ m = _.def_submodule("engine", "Core render engine.");

	UniversalSDLBinder binder(m);
	bind_engine(binder);
}

std::string UniversalSDLBinder::toSdlTypeName(nanobind::handle pyType)
{
	// See the nanobind doc for exchanging information
	// https://nanobind.readthedocs.io/en/latest/exchanging.html

	static const std::unordered_map<const PyObject*, std::string> pyTypeToSdlTypeName = 
		[]()
		{
			std::unordered_map<const PyObject*, std::string> map;

			map[nanobind::type<nanobind::bool_>().ptr()] = "bool";
			map[nanobind::type<nanobind::int_>().ptr()] = "integer";
			map[nanobind::type<nanobind::float_>().ptr()] = "real";
			map[nanobind::type<nanobind::str>().ptr()] = "string";

			auto pathlib = nanobind::module_::import_("pathlib");
			map[pathlib.attr("Path").ptr()] = "path";
			map[pathlib.attr("PurePosixPath").ptr()] = "path";
			map[pathlib.attr("PureWindowsPath").ptr()] = "path";
			if(nanobind::hasattr(pathlib, "PosixPath"))
			{
				map[pathlib.attr("PosixPath").ptr()] = "path";
			}
			if(nanobind::hasattr(pathlib, "WindowsPath"))
			{
				map[pathlib.attr("WindowsPath").ptr()] = "path";
			}

			return map;
		}();

	auto mapResult = pyTypeToSdlTypeName.find(pyType.ptr());
	if(mapResult != pyTypeToSdlTypeName.end())
	{
		return mapResult->second;
	}
	else
	{
		const std::string msg =
			"Unable to map Python value type <" +
			nanobind::cast<std::string>(nanobind::str(pyType)) +
			"> to SDL.";
		throw nanobind::type_error(msg.c_str());
	}
}

SdlInputClauses UniversalSDLBinder::toSdlInputClauses(nanobind::kwargs kwargs)
{
	SdlInputClauses clauses;
	for(auto [key, value] : kwargs)
	{
		clauses.add(
			SdlInputClause(
				toSdlTypeName(value.type()),
				nanobind::cast<std::string>(nanobind::str(key)),
				nanobind::cast<std::string>(nanobind::str(value))));
	}

	return clauses;
}

std::string UniversalSDLBinder::toRestructuredTextDocstring(const ISdlInstantiable& instantiableType)
{
	std::string docstring{instantiableType.getDescription()};
	docstring += "\n\n";
	for(std::size_t pi = 0; pi < instantiableType.numFields(); ++pi)
	{
		const SdlField* sdlField = instantiableType.getField(pi);
		docstring += ":param " + std::string(sdlField->getFieldName()) + ": ";
		docstring += std::string(sdlField->getDescription()) + "\n";
		docstring += ":type " + std::string(sdlField->getFieldName()) + ": ";
		docstring += std::string(sdlField->getTypeName()) + "\n";
	}

	return docstring;
}

}// end namespace ph::py
