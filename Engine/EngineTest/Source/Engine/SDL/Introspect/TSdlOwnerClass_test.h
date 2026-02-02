#pragma once

#include <Engine/SDL/sdl_interface.h>

namespace ph::TSdlOwnerClass_test
{

class TestResource : public ISdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_Geometry;

	real r;
	std::string str;

	ESdlTypeCategory getDynamicCategory() const override
	{
		return CATEGORY;
	}

	PH_DEFINE_SDL_CLASS(TestResource, clazz, outerScope=TSdlOwnerClass_test)
	{
		clazz.typeName("TSdlOwnerClass_test::testClass");
	}
};

struct TestMethodStruct
{
	void operator () (TestResource& res)
	{}

	PH_DEFINE_SDL_METHOD(TestMethodStruct, f, outerScope=TSdlOwnerClass_test)
	{
		f.name("TSdlOwnerClass_test::testMethod");
	}
};

struct TestMethodStruct2
{
	void operator () (const TestResource& res) const
	{}

	PH_DEFINE_SDL_METHOD(TestMethodStruct2, f, outerScope=TSdlOwnerClass_test)
	{
		f.name("TSdlOwnerClass_test::testMethod2");
	}
};

}// end namespace ph::TSdlOwnerClass_test