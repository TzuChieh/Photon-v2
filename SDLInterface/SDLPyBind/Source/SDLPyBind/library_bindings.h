#pragma once

/*! @file library_bindings.h
  
@brief Python binding generation utilities.

To add new bindings, basically you will need to do the following steps:
1. Create SDL definition for the C++ code and specify "interface=python".
2. Make sure the SDL definition is accessible to this project (public headers, linking).
3. Run setup script and regenerate the project through CMake.
4. Bind it here.

*/

#include "SDLPyBind/nanobind_exceptions.h"

#include <Engine/EEngineProject.h>
#include <Engine/SDL/sdl_meta.h>
#include <Engine/SDL/Introspect/SdlClass.h>
#include <Engine/SDL/Introspect/SdlFunction.h>
#include <Engine/SDL/Introspect/SdlField.h>
#include <Engine/SDL/Introspect/SdlInputContext.h>
#include <Engine/SDL/SdlInputClauses.h>
#include <Engine/SDL/ESdlDataFormat.h>
#include <Engine/SDL/ESdlDataType.h>
#include <Engine/SDL/sdl_helpers.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/shared_ptr.h>

#include <string>
#include <vector>
#include <utility>

namespace ph { class SceneDescription; }
namespace ph { class SdlNativeData; }

namespace ph::py
{

void bind_engine_sdl_definitions(nanobind::module_ m);

struct UniversalSDLBinder
{
	nanobind::module_ m;

	UniversalSDLBinder(nanobind::module_ m)
		: m(m)
	{}

	/*! @brief Bind a SDL class to Python.
	*/
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
			const std::string sdlFuncName = sdlFunc->getSnakeCaseName();
			const std::string docstring = toRestructuredTextDocstring(*sdlFunc);

			std::vector<const SdlField*> nativeAccessParams;
			for(std::size_t pi = 0; pi < sdlFunc->numParams(); ++pi)
			{
				if(sdlFunc->getParam(pi)->getOptions().hasAny(EFieldOption::PreferNativeAccess))
				{
					nativeAccessParams.push_back(sdlFunc->getParam(pi));
				}
			}

			if(sdlFunc->isStatic())
			{
				c.def_static(sdlFuncName.c_str(),
					[sdlFunc, nativeAccessParams](nanobind::kwargs kwargs)
					{
						callSdlStaticFunction(*sdlFunc, nativeAccessParams, kwargs);
					},
					docstring.c_str());
			}
			else
			{
				c.def(sdlFuncName.c_str(),
					[sdlFunc, nativeAccessParams](OwnerType& self, nanobind::kwargs kwargs)
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

	/*! @brief Convert a Python type to its corresponding SDL type name.
	*/
	static std::string toSdlTypeName(nanobind::handle pyType);

	/*! @brief Convert Python keyword arguments to SDL input clauses.
	*/
	static SdlInputClauses toSdlInputClauses(nanobind::kwargs kwargs);

	/*! @brief Try to transfer a Python value to SDL via native data interface.
	@return `true` if successful.
	*/
	static bool tryTransferToSdlNativeData(nanobind::handle pyValue, SdlNativeData& nativeData);

	/*! @brief Call a static SDL function.
	*/
	static void callSdlStaticFunction(
		const SdlFunction& sdlFunc,
		const std::vector<const SdlField*>& nativeAccessParams,
		nanobind::kwargs kwargs);

	/*! @brief Generate a reStructuredText docstring for a SDL instantiable type.
	*/
	static std::string toRestructuredTextDocstring(const ISdlInstantiable& instantiableType);
};

PH_DECLARE_DISPATCHER_FOR_ALL_SDL_CLASSES(bind_engine, UniversalSDLBinder, EEngineProject::Engine, outerScope=py);

}// end namespace ph::py
