#pragma once

#include <Engine/SDL/sdl_interface.h>
#include <Engine/Actor/Actor.h>

namespace ph::TSdlAnyInstance_test
{

struct DummyStruct
{
	int x = 0;

	PH_DEFINE_SDL_STRUCT(DummyStruct, ztruct, outerScope=TSdlAnyInstance_test)
	{
		ztruct.typeName("TSdlAnyInstance_test::dummy");
	}
};

struct DummyMethodStruct
{
	int p = 0;

	void operator () (Actor& res)
	{}

	PH_DEFINE_SDL_FUNCTION(DummyMethodStruct, f, outerScope=TSdlAnyInstance_test)
	{
		f.name("TSdlAnyInstance_test::dummy");
	}
};

}// end namespace ph::TSdlAnyInstance_test
