#include <SDL/sdl_traits.h>
#include <SDL/sdl_interface.h>
#include <Actor/Actor.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{

class DummyClass
{};

class DummySdlClass : public ISdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_Object;

	ESdlTypeCategory getDynamicCategory() const override
	{
		return CATEGORY;
	}

	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<DummySdlClass>)
	{
		ClassType clazz("dummy");
		return clazz;
	}
};

class DummySdlClass2 : public DummySdlClass
{
public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<DummySdlClass2>)
	{
		ClassType clazz("dummy2");
		return clazz;
	}
};

class DummySdlClassDerived : public DummySdlClass
{};

struct DummyStruct
{};

struct DummySdlStruct
{
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<DummySdlStruct>)
	{
		StructType ztruct("dummy");
		return ztruct;
	}
};

struct DummySdlStruct2 : public DummySdlStruct
{
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<DummySdlStruct2>)
	{
		StructType ztruct("dummy2");
		return ztruct;
	}
};

struct DummySdlStructDerived : public DummySdlStruct
{};

struct DummyMethodStruct
{};

struct DummySdlMethodStruct
{
	void operator () (DummySdlClass& res)
	{}

	PH_DEFINE_SDL_FUNCTION(TSdlMethod<DummySdlMethodStruct, DummySdlClass>)
	{
		return FunctionType("dummy");
	}
};

struct DummySdlMethodStruct2 : public DummySdlMethodStruct
{
	PH_DEFINE_SDL_FUNCTION(TSdlMethod<DummySdlMethodStruct2, DummySdlClass>)
	{
		return FunctionType("dummy2");
	}
};

struct DummySdlMethodStructDerived : public DummySdlMethodStruct
{};

enum class EDummy
{};

enum class EDummySdl
{};

}// end anonymous namespace

namespace ph
{

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EDummySdl>)
{
	SdlEnumType sdlEnum("dummy");
	return sdlEnum;
}

}// end namespace ph

TEST(SdlTraitsTest, SdlClassTraits)
{
	static_assert(CHasSdlClassDefinition<DummyClass> == false);
	static_assert(CHasSdlClassDefinition<DummySdlClass> == true);
	static_assert(CHasSdlClassDefinition<Actor> == true);
	static_assert(CHasSdlClassDefinition<DummyStruct> == false);
	static_assert(CHasSdlClassDefinition<DummySdlStruct> == false);
	static_assert(CHasSdlClassDefinition<DummyMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<DummySdlMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlClassDefinition<TSdlEnum<EDummySdl>> == false);
	static_assert(CHasSdlClassDefinition<int> == false);
	static_assert(CHasSdlClassDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlClassDefinition<const DummyClass> == false);
	static_assert(CHasSdlClassDefinition<const DummySdlClass> == true);
	static_assert(CHasSdlClassDefinition<const Actor> == true);
	static_assert(CHasSdlClassDefinition<const DummyStruct> == false);
	static_assert(CHasSdlClassDefinition<const DummySdlStruct> == false);
	static_assert(CHasSdlClassDefinition<const DummyMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<const DummySdlMethodStruct> == false);
	static_assert(CHasSdlClassDefinition<const TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlClassDefinition<const TSdlEnum<EDummySdl>> == false);
	static_assert(CHasSdlClassDefinition<const int> == false);
	static_assert(CHasSdlClassDefinition<const float> == false);

	// Must not be fooled by a derived class without SDL definition
	static_assert(CHasSdlClassDefinition<DummySdlClassDerived> == false);
	static_assert(CHasSdlClassDefinition<DummySdlClass2> == true);
}

TEST(SdlTraitsTest, SdlStructTraits)
{
	static_assert(CHasSdlStructDefinition<DummyStruct> == false);
	static_assert(CHasSdlStructDefinition<DummySdlStruct> == true);
	static_assert(CHasSdlStructDefinition<DummyClass> == false);
	static_assert(CHasSdlStructDefinition<DummySdlClass> == false);
	static_assert(CHasSdlStructDefinition<Actor> == false);
	static_assert(CHasSdlStructDefinition<DummyMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<DummySdlMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlStructDefinition<TSdlEnum<EDummySdl>> == false);
	static_assert(CHasSdlStructDefinition<int> == false);
	static_assert(CHasSdlStructDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlStructDefinition<const DummyStruct> == false);
	static_assert(CHasSdlStructDefinition<const DummySdlStruct> == true);
	static_assert(CHasSdlStructDefinition<const DummyClass> == false);
	static_assert(CHasSdlStructDefinition<const DummySdlClass> == false);
	static_assert(CHasSdlStructDefinition<const Actor> == false);
	static_assert(CHasSdlStructDefinition<const DummyMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<const DummySdlMethodStruct> == false);
	static_assert(CHasSdlStructDefinition<const TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlStructDefinition<const TSdlEnum<EDummySdl>> == false);
	static_assert(CHasSdlStructDefinition<const int> == false);
	static_assert(CHasSdlStructDefinition<const float> == false);

	// Must not be fooled by a derived struct without SDL definition
	static_assert(CHasSdlStructDefinition<DummySdlStructDerived> == false);
	static_assert(CHasSdlStructDefinition<DummySdlStruct2> == true);
}

TEST(SdlTraitsTest, SdlFunctionTraits)
{
	static_assert(CHasSdlFunctionDefinition<DummyMethodStruct> == false);
	static_assert(CHasSdlFunctionDefinition<DummySdlMethodStruct> == true);
	static_assert(CHasSdlFunctionDefinition<DummyClass> == false);
	static_assert(CHasSdlFunctionDefinition<DummySdlClass> == false);
	static_assert(CHasSdlFunctionDefinition<Actor> == false);
	static_assert(CHasSdlFunctionDefinition<DummyStruct> == false);
	static_assert(CHasSdlFunctionDefinition<DummySdlStruct> == false);
	static_assert(CHasSdlFunctionDefinition<TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlFunctionDefinition<TSdlEnum<EDummySdl>> == false);
	static_assert(CHasSdlFunctionDefinition<int> == false);
	static_assert(CHasSdlFunctionDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlFunctionDefinition<const DummyMethodStruct> == false);
	static_assert(CHasSdlFunctionDefinition<const DummySdlMethodStruct> == true);
	static_assert(CHasSdlFunctionDefinition<const DummyClass> == false);
	static_assert(CHasSdlFunctionDefinition<const DummySdlClass> == false);
	static_assert(CHasSdlFunctionDefinition<const Actor> == false);
	static_assert(CHasSdlFunctionDefinition<const DummyStruct> == false);
	static_assert(CHasSdlFunctionDefinition<const DummySdlStruct> == false);
	static_assert(CHasSdlFunctionDefinition<const TSdlEnum<EDummy>> == false);
	static_assert(CHasSdlFunctionDefinition<const TSdlEnum<EDummySdl>> == false);
	static_assert(CHasSdlFunctionDefinition<const int> == false);
	static_assert(CHasSdlFunctionDefinition<const float> == false);

	// Must not be fooled by a derived struct without SDL definition
	static_assert(CHasSdlFunctionDefinition<DummySdlMethodStructDerived> == false);
	static_assert(CHasSdlFunctionDefinition<DummySdlMethodStruct2> == true);
}

TEST(SdlTraitsTest, SdlEnumTraits)
{
	static_assert(CHasSdlEnumDefinition<EDummy> == false);
	static_assert(CHasSdlEnumDefinition<EDummySdl> == true);
	static_assert(CHasSdlEnumDefinition<DummyMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<DummySdlMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<DummyClass> == false);
	static_assert(CHasSdlEnumDefinition<DummySdlClass> == false);
	static_assert(CHasSdlEnumDefinition<Actor> == false);
	static_assert(CHasSdlEnumDefinition<DummyStruct> == false);
	static_assert(CHasSdlEnumDefinition<DummySdlStruct> == false);
	static_assert(CHasSdlEnumDefinition<int> == false);
	static_assert(CHasSdlEnumDefinition<float> == false);

	// Must also recognize const variants
	static_assert(CHasSdlEnumDefinition<const EDummy> == false);
	static_assert(CHasSdlEnumDefinition<const EDummySdl> == true);
	static_assert(CHasSdlEnumDefinition<const DummyMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<const DummySdlMethodStruct> == false);
	static_assert(CHasSdlEnumDefinition<const DummyClass> == false);
	static_assert(CHasSdlEnumDefinition<const DummySdlClass> == false);
	static_assert(CHasSdlEnumDefinition<const Actor> == false);
	static_assert(CHasSdlEnumDefinition<const DummyStruct> == false);
	static_assert(CHasSdlEnumDefinition<const DummySdlStruct> == false);
	static_assert(CHasSdlEnumDefinition<const int> == false);
	static_assert(CHasSdlEnumDefinition<const float> == false);
}
