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

	PH_DEFINE_SDL_CLASS(DummyClass, clazz, outerScope=sdl_traits_test)
	{
		clazz.typeName("sdl_traits_test::dummy");
	}
};

class DummyClass2 : public DummyClass
{
public:
	PH_DEFINE_SDL_CLASS(DummyClass2, clazz, outerScope=sdl_traits_test)
	{
		clazz.typeName("sdl_traits_test::dummy2");
	}
};

class DummyClassDerived : public DummyClass
{};

struct NonSdlDummyStruct
{};

struct DummyStruct
{
	PH_DEFINE_SDL_STRUCT(DummyStruct, ztruct, outerScope=sdl_traits_test)
	{
		ztruct.typeName("sdl_traits_test::dummy");
	}
};

struct DummyStruct2 : public DummyStruct
{
	PH_DEFINE_SDL_STRUCT(DummyStruct2, ztruct, outerScope=sdl_traits_test)
	{
		ztruct.typeName("sdl_traits_test::dummy2");
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

	PH_DEFINE_SDL_METHOD(DummyMethodStruct, func, outerScope=sdl_traits_test)
	{
		func.name("sdl_traits_test::dummy");
	}
};

struct DummyMethodStruct2 : public DummyMethodStruct
{
	PH_DEFINE_SDL_METHOD(DummyMethodStruct2, func, outerScope=sdl_traits_test)
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

PH_DEFINE_SDL_ENUM(sdl_traits_test::EDummy, e)
{
	e.name("sdl_traits_test::dummy");
}

}// end namespace ph
