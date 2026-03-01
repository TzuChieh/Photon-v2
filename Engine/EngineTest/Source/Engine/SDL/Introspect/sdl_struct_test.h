#pragma once

#include <Engine/SDL/sdl_interface.h>

namespace ph::sdl_struct_test
{

struct EmptyStruct
{
	PH_DEFINE_SDL_STRUCT(EmptyStruct, ztruct, outerScope=sdl_struct_test)
	{
		ztruct.typeName("sdl_struct_test::dummy");
	}
};

}// end namespace ph::sdl_struct_test
