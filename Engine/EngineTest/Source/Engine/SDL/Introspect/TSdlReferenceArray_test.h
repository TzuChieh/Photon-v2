#pragma once

#include <Engine/SDL/TSdlResourceBase.h>
#include <Engine/SDL/sdl_interface.h>

#include <memory>
#include <vector>

namespace ph::TSdlReferenceArray_test
{

class TestResource final : public TSdlResourceBase<ESdlTypeCategory::Ref_Object>
{
public:
	PH_DEFINE_SDL_CLASS(TestResource, clazz, outerScope=TSdlReferenceArray_test)
	{
		clazz.typeName("TSdlReferenceArray_test::test-resource");
	}
};

class TestReferenceArrayOwner
{
public:
	std::vector<std::shared_ptr<TestResource>> values;
};

}// end namespace ph::TSdlReferenceArray_test

