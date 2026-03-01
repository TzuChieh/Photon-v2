#pragma once

#include <Engine/SDL/Introspect/TSdlOwnerMethod.h>
#include <Engine/SDL/TSdlResourceBase.h>
#include <Engine/SDL/sdl_interface.h>

namespace ph::TSdlOwnerMethod_test
{

class TestResource : public TSdlResourceBase<ESdlTypeCategory::Ref_Option>
{
public:
	std::string str;

	PH_DEFINE_SDL_CLASS(TestResource, clazz, outerScope=TSdlOwnerMethod_test)
	{
		clazz.typeName("TSdlOwnerMethod_test::dummy");
	}
};

struct TestMethodStruct
{
	std::string str;

	void operator () (TestResource& res)
	{
		res.str = str;
	}

	PH_DEFINE_SDL_STRUCT(TestMethodStruct, ztruct, outerScope=TSdlOwnerMethod_test)
	{
		ztruct.typeName("TSdlOwnerMethod_test::dummy2");
	}
};

}// end namespace ph::TSdlOwnerMethod_test
