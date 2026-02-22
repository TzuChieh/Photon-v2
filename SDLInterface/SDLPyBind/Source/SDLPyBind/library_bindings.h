#pragma once

/*! @file library_bindings.h
  
@brief Python binding generation utilities.

To add new bindings, basically you will need to do the following steps:
1. Create SDL definition for the C++ code and specify "interface=python".
2. Make sure the SDL definition is accessible to this project (public headers, linking).
3. Run setup script and regenerate the project through CMake.
4. Bind it here.

*/

#include <Engine/EEngineProject.h>
#include <Engine/SDL/sdl_meta.h>
#include <Engine/SDL/Introspect/SdlClass.h>
#include <Engine/SDL/Introspect/SdlFunction.h>
#include <Engine/SDL/Introspect/SdlField.h>
#include <Engine/SDL/SdlInputClauses.h>
#include <Engine/SDL/sdl_helpers.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/shared_ptr.h>

#include <string>

namespace ph { class SceneDescription; }

namespace ph::py
{

void bind_engine_sdl_definitions(nanobind::module_ m);

struct UniversalSDLBinder
{
	nanobind::module_ m;

	UniversalSDLBinder(nanobind::module_ m)
		: m(m)
	{}

	template<typename StaticSDLClassType>
	void operator () (const StaticSDLClassType& sdlClass) const
	{
		const bool isPythonBindingRequested = sdlClass.getUserSpec().hasArg("interface", "python");
		if(!isPythonBindingRequested)
		{
			return;
		}

		using OwnerType = typename StaticSDLClassType::OwnerType;

		auto c = nanobind::class_<OwnerType>(
			m,
			sdlClass.getUserSpec().getArg(0).c_str(),
			std::string(sdlClass.getDescription()).c_str());

		// TODO: should use a proper SdlInputContext to resolve SDL names, wd, etc.

		if(!sdlClass.isBlueprint() && sdlClass.allowCreateFromClass())
		{
			const std::string docstring = toRestructuredTextDocstring(sdlClass);

			// nanobind caches object construction method, so we do not need to bind init
			// (https://nanobind.readthedocs.io/en/latest/classes.html#customizing-python-object-creation)
			c.def(nanobind::new_(
				[&sdlClass](nanobind::kwargs kwargs)
				{
					std::shared_ptr<ISdlResource> resource = sdlClass.createResource();
					if(kwargs.empty())
					{
						sdlClass.initDefaultResource(*resource);
					}
					else
					{
						SdlInputClauses clauses = toSdlInputClauses(kwargs);
						sdlClass.initResource(
							*resource,
							clauses,
							SdlInputContext{});
					}

					return std::static_pointer_cast<OwnerType>(resource);
				}),
				docstring.c_str());
		}

		for(std::size_t fi = 0; fi < sdlClass.numFunctions(); ++fi)
		{
			const SdlFunction* sdlFunc = sdlClass.getFunction(fi);
			const std::string sdlFuncName = sdl::name_to_snake_case(sdlFunc->getTypeName());
			const std::string docstring = toRestructuredTextDocstring(*sdlFunc);

			if(sdlFunc->isStatic())
			{
				c.def_static(sdlFuncName.c_str(),
					[sdlFunc](nanobind::kwargs kwargs)
					{
						SdlInputClauses clauses = toSdlInputClauses(kwargs);
						sdlFunc->call(
							nullptr,
							clauses,
							SdlInputContext{});
					},
					docstring.c_str());
			}
			else
			{
				c.def(sdlFuncName.c_str(),
					[sdlFunc](OwnerType& self, nanobind::kwargs kwargs)
					{
						SdlInputClauses clauses = toSdlInputClauses(kwargs);
						sdlFunc->call(
							&self,
							clauses,
							SdlInputContext{});
					},
					docstring.c_str());
			}
		}
	}

	static const nanobind::object& pyPurePathType()
	{
		static nanobind::object purePathType = nanobind::module_::import_("pathlib").attr("PurePath");
		return purePathType;
	}

	static std::string toSdlTypeName(nanobind::handle pyValue);
	static SdlInputClauses toSdlInputClauses(nanobind::kwargs kwargs);
	static std::string toRestructuredTextDocstring(const ISdlInstantiable& instantiableType);
};

PH_DECLARE_DISPATCHER_FOR_ALL_SDL_CLASSES(bind_engine, UniversalSDLBinder, EEngineProject::Engine, outerScope=py);

}// end namespace ph::py
