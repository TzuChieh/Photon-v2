#pragma once

#include "Engine/SDL/sdl_fwd.h"
#include "Engine/EEngineProject.h"

#include <vector>

/*! @brief Declares a function that returns all public SDL classes in current library/project.
Signature: `std::vector<const ph::SdlClass*> getterFuncName()`.
Public SDL classes are those defined in header.
@param getterFuncName The name of the function to be declared.
@note Supports namespace scope only when specifying `outerScope`. Function definition will
be automatically generated.
*/
#define PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES(getterFuncName, ...)\
	std::vector<const ::ph::SdlClass*> getterFuncName()

/*! @brief Declares a function that returns all public SDL enums in current library/project.
Signature: `std::vector<const ph::SdlEnum*> getterFuncName()`.
Public SDL enums are those defined in header.
@param getterFuncName The name of the function to be declared.
@note Supports namespace scope only when specifying `outerScope`. Function definition will
be automatically generated.
*/
#define PH_DECLARE_GETTER_FOR_ALL_SDL_ENUMS(getterFuncName, ...)\
	std::vector<const ::ph::SdlEnum*> getterFuncName()

/*! @brief Declares a function that visits all public SDL classes in any library/project.
Signature: `void dispatcherFuncName(visitorType&& visitor)`.
Public SDL classes are those defined in header.
@param dispatcherFuncName The name of the function to be declared. This function will call the visitor
with all specified SDL class instances.
@param visitorType Type of the visitor. It should be a callable type accepting a single parameter
of the SDL class instance type, i.e., callable as `visitor(classInstance)`, where the input
`classInstance` has the same type (static SDL class type) as obtained by `OwnerType::getSdlClass()`.
@param project A project specifier of type `ph::EEngineProject`. Specifies which project's SDL classes to target.
@note Supports namespace scope only when specifying `outerScope`. Function definition will
be automatically generated. This is a fairly low-level API and it's user's responsibility to ensure
the target SDL classes are accessible (for example: linking, scopes).
*/
#define PH_DECLARE_DISPATCHER_FOR_ALL_SDL_CLASSES(dispatcherFuncName, visitorType, project, ...)\
	void dispatcherFuncName(visitorType&& visitor)