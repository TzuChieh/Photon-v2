#pragma once

#include <Engine/SDL/sdl_interface.h>

namespace ph::sdl_traits_test
{

class NonSdlDummyClass
{};

class DummyClass : public ISdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_Object;

	ESdlTypeCategory getDynamicCategory() const override
	{
		return CATEGORY;
	}

	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<DummyClass>)
	{
		ClassType clazz("sdl_traits_test::dummy");
		return clazz;
	}
};

class DummyClass2 : public DummyClass
{
public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<DummyClass2>)
	{
		ClassType clazz("sdl_traits_test::dummy2");
		return clazz;
	}
};

class DummyClassDerived : public DummyClass
{};

struct NonSdlDummyStruct
{};

struct DummyStruct
{
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<DummyStruct>)
	{
		StructType ztruct("sdl_traits_test::dummy");
		return ztruct;
	}
};

struct DummyStruct2 : public DummyStruct
{
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<DummyStruct2>)
	{
		StructType ztruct("sdl_traits_test::dummy2");
		return ztruct;
	}
};

struct DummyStructDerived : public DummyStruct
{};

struct NonSdlDummyMethodStruct
{};

struct DummyMethodStruct
{
	void operator () (DummyClass& res)
	{}

	PH_DEFINE_SDL_FUNCTION(DummyMethodStruct, func, outerScope=sdl_traits_test)
	{
		func.name("sdl_traits_test::dummy");
	}
};

struct DummyMethodStruct2 : public DummyMethodStruct
{
	PH_DEFINE_SDL_FUNCTION(DummyMethodStruct2, func, outerScope=sdl_traits_test)
	{
		func.name("sdl_traits_test::dummy2");
	}
};

struct DummyMethodStructDerived : public DummyMethodStruct
{};

enum class ENonSdlDummy
{};

enum class EDummy
{};

}// end namespace ph::sdl_traits_test

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<sdl_traits_test::EDummy>)
{
	SdlEnumType sdlEnum("sdl_traits_test::dummy");
	return sdlEnum;
}

}// end namespace ph
