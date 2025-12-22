#include "Engine/SDL/sdl_traits_test.h"

#include <Engine/SDL/sdl_traits.h>
#include <Engine/Actor/Actor.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::sdl_traits_test;

TEST(SdlTraitsTest, SdlClassTraits)
{
	static_assert(CHasSdlClassDefinition<NonSdlDummyClass> == false);
	static_assert(CHasSdlClassDefinition<DummyClass> == true);
	static_assert(CHasSdlClassDefinition<Actor> == true);
	static_assert(CHasSdlClassDefinition<NonSdlDummyStruct> == false);
	static_assert(CHasSdlClassDefinition<DummyStruct> == false);
	static_assert(CHasSdlClassDefinition<NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<DummyMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<TSdlEnum<ENonSdlDummy>> == false);
	static_assert(CHasSdlClassDefinition<TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlClassDefinition<int> == false);
	static_assert(CHasSdlClassDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlClassDefinition<const NonSdlDummyClass> == false);
	static_assert(CHasSdlClassDefinition<const DummyClass> == true);
	static_assert(CHasSdlClassDefinition<const Actor> == true);
	static_assert(CHasSdlClassDefinition<const NonSdlDummyStruct> == false);
	static_assert(CHasSdlClassDefinition<const DummyStruct> == false);
	static_assert(CHasSdlClassDefinition<const NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<const DummyMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<const TSdlEnum<ENonSdlDummy>> == false);
	static_assert(CHasSdlClassDefinition<const TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlClassDefinition<const int> == false);
	static_assert(CHasSdlClassDefinition<const float> == false);

	// Must not be fooled by a derived class without SDL definition
	static_assert(CHasSdlClassDefinition<DummyClassDerived> == false);
	static_assert(CHasSdlClassDefinition<DummyClass2> == true);
}

TEST(SdlTraitsTest, SdlStructTraits)
{
	static_assert(CHasSdlStructDefinition<NonSdlDummyStruct> == false);
	static_assert(CHasSdlStructDefinition<DummyStruct> == true);
	static_assert(CHasSdlStructDefinition<NonSdlDummyClass> == false);
	static_assert(CHasSdlStructDefinition<DummyClass> == false);
	static_assert(CHasSdlStructDefinition<Actor> == false);
	static_assert(CHasSdlStructDefinition<NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<DummyMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<TSdlEnum<ENonSdlDummy>> == false);
	static_assert(CHasSdlStructDefinition<TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlStructDefinition<int> == false);
	static_assert(CHasSdlStructDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlStructDefinition<const NonSdlDummyStruct> == false);
	static_assert(CHasSdlStructDefinition<const DummyStruct> == true);
	static_assert(CHasSdlStructDefinition<const NonSdlDummyClass> == false);
	static_assert(CHasSdlStructDefinition<const DummyClass> == false);
	static_assert(CHasSdlStructDefinition<const Actor> == false);
	static_assert(CHasSdlStructDefinition<const NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<const DummyMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<const TSdlEnum<ENonSdlDummy>> == false);
	static_assert(CHasSdlStructDefinition<const TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlStructDefinition<const int> == false);
	static_assert(CHasSdlStructDefinition<const float> == false);

	// Must not be fooled by a derived struct without SDL definition
	static_assert(CHasSdlStructDefinition<DummyStructDerived> == false);
	static_assert(CHasSdlStructDefinition<DummyStruct2> == true);
}

TEST(SdlTraitsTest, SdlFunctionTraits)
{
	static_assert(CHasSdlFunctionDefinition<NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlFunctionDefinition<DummyMethodStruct> == true);
	static_assert(CHasSdlFunctionDefinition<NonSdlDummyClass> == false);
	static_assert(CHasSdlFunctionDefinition<DummyClass> == false);
	static_assert(CHasSdlFunctionDefinition<Actor> == false);
	static_assert(CHasSdlFunctionDefinition<NonSdlDummyStruct> == false);
	static_assert(CHasSdlFunctionDefinition<DummyStruct> == false);
	static_assert(CHasSdlFunctionDefinition<TSdlEnum<ENonSdlDummy>> == false);
	static_assert(CHasSdlFunctionDefinition<TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlFunctionDefinition<int> == false);
	static_assert(CHasSdlFunctionDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlFunctionDefinition<const NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlFunctionDefinition<const DummyMethodStruct> == true);
	static_assert(CHasSdlFunctionDefinition<const NonSdlDummyClass> == false);
	static_assert(CHasSdlFunctionDefinition<const DummyClass> == false);
	static_assert(CHasSdlFunctionDefinition<const Actor> == false);
	static_assert(CHasSdlFunctionDefinition<const NonSdlDummyStruct> == false);
	static_assert(CHasSdlFunctionDefinition<const DummyStruct> == false);
	static_assert(CHasSdlFunctionDefinition<const TSdlEnum<ENonSdlDummy>> == false);
	static_assert(CHasSdlFunctionDefinition<const TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlFunctionDefinition<const int> == false);
	static_assert(CHasSdlFunctionDefinition<const float> == false);

	// Must not be fooled by a derived struct without SDL definition
	static_assert(CHasSdlFunctionDefinition<DummyMethodStructDerived> == false);
	static_assert(CHasSdlFunctionDefinition<DummyMethodStruct2> == true);
}

TEST(SdlTraitsTest, SdlEnumTraits)
{
	static_assert(CHasSdlEnumDefinition<ENonSdlDummy> == false);
	static_assert(CHasSdlEnumDefinition<EDummy> == true);
	static_assert(CHasSdlEnumDefinition<NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<DummyMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<NonSdlDummyClass> == false);
	static_assert(CHasSdlEnumDefinition<DummyClass> == false);
	static_assert(CHasSdlEnumDefinition<Actor> == false);
	static_assert(CHasSdlEnumDefinition<NonSdlDummyStruct> == false);
	static_assert(CHasSdlEnumDefinition<DummyStruct> == false);
	static_assert(CHasSdlEnumDefinition<int> == false);
	static_assert(CHasSdlEnumDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlEnumDefinition<const ENonSdlDummy> == false);
	static_assert(CHasSdlEnumDefinition<const EDummy> == true);
	static_assert(CHasSdlEnumDefinition<const NonSdlDummyMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<const DummyMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<const NonSdlDummyClass> == false);
	static_assert(CHasSdlEnumDefinition<const DummyClass> == false);
	static_assert(CHasSdlEnumDefinition<const Actor> == false);
	static_assert(CHasSdlEnumDefinition<const NonSdlDummyStruct> == false);
	static_assert(CHasSdlEnumDefinition<const DummyStruct> == false);
	static_assert(CHasSdlEnumDefinition<const int> == false);
	static_assert(CHasSdlEnumDefinition<const float> == false);
}
