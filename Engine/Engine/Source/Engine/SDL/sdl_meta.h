#pragma once

#include "Engine/SDL/sdl_fwd.h"

#include <vector>

/*!
Declares a function that returns all public SDL classes in current library/project.
Signature: `std::vector<const ph::SdlClass*> getterFuncName()`.
Public SDL classes are those defined in headeer.
@param getterFuncName The name of the function to be declared.
@note Supports namespace scope only when specifying `outerScope`.
*/
#define PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES(getterFuncName, ...)\
	std::vector<const ::ph::SdlClass*> getterFuncName();

/*!
Declares a function that returns all public SDL enums in current library/project.
Signature: `std::vector<const ph::SdlEnum*> getterFuncName()`.
Public SDL enums are those defined in headeer.
@param getterFuncName The name of the function to be declared.
@note Supports namespace scope only when specifying `outerScope`.
*/
#define PH_DECLARE_GETTER_FOR_ALL_SDL_ENUMS(getterFuncName, ...)\
	std::vector<const ::ph::SdlEnum*> getterFuncName();
