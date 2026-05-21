#pragma once

#include <Engine/SDL/sdl_interface.h>

#include <vector>

namespace ph::TSdlStructArray_test
{

struct TestStruct
{
	integer value = 0;

	PH_DEFINE_SDL_STRUCT(TestStruct, ztruct, outerScope=TSdlStructArray_test)
	{
		ztruct.typeName("TSdlStructArray_test::test-struct");

		TSdlInteger<OwnerType> value("value", &OwnerType::value);
		value.required();
		ztruct.addField(value);
	}
};

class TestStructArrayOwner
{
public:
	std::vector<TestStruct> values;
};

}// end namespace ph::TSdlStructArray_test
