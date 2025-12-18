#pragma once

#include "Engine/SDL/sdl_fwd.h"

#include <vector>

/*!
Declares a function that returns all SDL classes registered in the API database.
Signature: `std::vector<const ph::SdlClass*> getterFuncName()`.
@param getterFuncName The name of the function to be declared.
*/
#define PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES(getterFuncName, ...)\
	std::vector<const ::ph::SdlClass*> getterFuncName();
