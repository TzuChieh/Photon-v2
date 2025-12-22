#pragma once

#include "Engine/SDL/sdl_fwd.h"

#include <vector>

/*!
Declares a function that returns all public SDL classes.
Signature: `std::vector<const ph::SdlClass*> getterFuncName()`.
Public SDL classes are those defined in headeer.
@param getterFuncName The name of the function to be declared.
*/
#define PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES(getterFuncName, ...)\
	std::vector<const ::ph::SdlClass*> getterFuncName();
